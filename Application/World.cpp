#include <thread>

#include <ScopCore.h>

#include <Block.h>
#include <World.h>
#include <Utils.h>

World::World(Scop::Scene& scene) : m_noisecollection(42), p_water_pipeline(std::make_shared<Scop::GraphicPipeline>()), m_fps_counter(), m_scene(scene), m_previous_chunk_position(-1000, 10000)
{
	p_water_vertex_shader = Scop::LoadShaderFromFile(GetExecutablePath().parent_path().parent_path() / "Resources/Shaders/Build/WaterVertex.spv", Scop::ShaderType::Vertex, Scop::DefaultForwardVertexShaderLayout);
	p_water_fragment_shader = Scop::LoadShaderFromFile(GetExecutablePath().parent_path().parent_path() / "Resources/Shaders/Build/WaterFragment.spv", Scop::ShaderType::Fragment, Scop::DefaultShaderLayout);

	Scop::GraphicPipelineDescriptor pipeline_descriptor;
	pipeline_descriptor.vertex_shader = p_water_vertex_shader;
	pipeline_descriptor.fragment_shader = p_water_fragment_shader;
	pipeline_descriptor.clear_color_attachments = false;
	pipeline_descriptor.name = "water_forward_pass_pipeline";
	p_water_pipeline->Setup(pipeline_descriptor);

	Scop::Vec2ui32 map_size;
	Scop::MaterialTextures material_params;
	material_params.albedo = std::make_shared<Scop::Texture>(Scop::LoadBMPFile(GetResourcesPath() / "atlas.bmp", map_size), map_size.x, map_size.y);
	p_block_material = std::make_shared<Scop::Material>(material_params);

	scene.LoadFont(GetResourcesPath() / "Font.ttf", 16.0f);
	Scop::Text& fps_text = scene.CreateText("FPS:");
	fps_text.SetPosition(Scop::Vec2ui{ 30, 30 });

	Scop::Text& copyright_text = scene.CreateText("Copyright maldavid Studios");
	copyright_text.SetScale(Scop::Vec2f{ 0.75f });

	std::thread(&World::GenerateWorld, this).detach();

	SetupLoading();

	auto narrator_update = [this, &copyright_text](Scop::NonOwningPtr<Scop::Scene> scene, Scop::Inputs& input, float delta)
	{
		static bool generate = true;
		static bool generation_debounce = false;
		static bool wireframe_debounce = false;
		static PostProcessData post_process_data;

		copyright_text.SetPosition(Scop::Vec2ui{ Scop::ScopEngine::Get().GetWindow().GetWidth() - 175, Scop::ScopEngine::Get().GetWindow().GetHeight() - 20 });

		m_fps_counter.Update();
		if(m_fps_counter.GetFPSCount() != m_last_fps_count)
		{
			m_last_fps_count = m_fps_counter.GetFPSCount();
			if(p_fps_text)
				m_scene.RemoveText(*p_fps_text);
			p_fps_text = &m_scene.CreateText(std::to_string(m_last_fps_count));
			p_fps_text->SetPosition(Scop::Vec2ui{ 80, 30 });
		}

		Scop::FirstPerson3D* camera = reinterpret_cast<Scop::FirstPerson3D*>(m_scene.GetCamera().get());
		Scop::Vec3f camera_position = camera->GetPosition();
		Scop::Vec2i global_block_position = Scop::Vec2i{
			static_cast<std::int32_t>(camera_position.x < 0.0f ? camera_position.x - 1.0f : camera_position.x),
			static_cast<std::int32_t>(camera_position.z < 0.0f ? camera_position.z - 1.0f : camera_position.z),
		};

		m_current_chunk_position = Scop::Vec2i{
			static_cast<std::int32_t>(global_block_position.x < 0 ? std::floorf(global_block_position.x / static_cast<float>(CHUNK_SIZE.x)) : static_cast<float>(global_block_position.x) / static_cast<std::int32_t>(CHUNK_SIZE.x)),
			static_cast<std::int32_t>(global_block_position.y < 0 ? std::floorf(global_block_position.y / static_cast<float>(CHUNK_SIZE.z)) : static_cast<float>(global_block_position.y) / static_cast<std::int32_t>(CHUNK_SIZE.z)),
		};

		if(Scop::NonOwningPtr<Chunk> current_chunk = GetChunk(m_current_chunk_position); current_chunk)
		{
			Scop::Vec3i block_position = Scop::Vec3i{
				global_block_position.x - (m_current_chunk_position.x * static_cast<std::int32_t>(CHUNK_SIZE.x)),
				static_cast<std::int32_t>(camera_position.y),
				global_block_position.y - (m_current_chunk_position.y * static_cast<std::int32_t>(CHUNK_SIZE.z)),
			};
			post_process_data.underwater = current_chunk->GetBlock(block_position) == static_cast<std::uint32_t>(BlockType::Water);
		}

		post_process_data.inv_res = Scop::Vec2f{ 1.0f / Scop::ScopEngine::Get().GetWindow().GetWidth(), 1.0f / Scop::ScopEngine::Get().GetWindow().GetHeight() };

		std::memcpy(scene->GetPostProcessData().data.GetDataAs<PostProcessData>(), &post_process_data, sizeof(PostProcessData));

		if(input.IsKeyPressed(SDL_SCANCODE_G))
			generation_debounce = true;
		else if(generation_debounce)
		{
			generate = !generate;
			generation_debounce = false;
		}

		if(generate)
		{
			if(m_generation_status != GenerationState::Ready || m_current_chunk_position != m_previous_chunk_position)
			{
				if(m_generation_status == GenerationState::Ready)
				{
					UnloadChunks();
					m_generation_status = GenerationState::Working;
				}
				else if(m_generation_status == GenerationState::Finished)
				{
					m_generation_status = GenerationState::Ready;
					m_show_loading_screen = false;
				}
				m_previous_chunk_position = m_current_chunk_position;
			}
			Upload();
		}

		if(input.IsKeyPressed(SDL_SCANCODE_F))
			wireframe_debounce = true;
		else if(wireframe_debounce)
		{
			m_scene.GetForwardData().wireframe = !m_scene.GetForwardData().wireframe;
			Scop::RenderCore::Get().WaitDeviceIdle();
			m_scene.GetPipeline().Destroy();
			wireframe_debounce = false;
		}
	};

	m_scene.CreateNarrator().AttachScript(std::make_shared<Scop::NativeNarratorScript>(std::function<void()>{}, narrator_update, std::function<void()>{}));
}

World::~World()
{
	m_generation_status = GenerationState::Quitting;
	while(m_generation_status != GenerationState::Finished)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(16ms);
	}
}

[[nodiscard]] Scop::NonOwningPtr<Chunk> World::GetChunk(Scop::Vec2i position)
{
	auto it = m_chunks.find(position);
	if(it == m_chunks.end())
		return nullptr;
	return &it->second;
}

void World::UnloadChunks()
{
	for(auto it = m_chunks.begin(); it != m_chunks.end();)
	{
		Scop::Vec2i pos = it->first;
		std::uint32_t x_dist = std::abs(pos.x - m_current_chunk_position.x);
		std::uint32_t z_dist = std::abs(pos.y - m_current_chunk_position.y);
		if(RENDER_DISTANCE < x_dist || RENDER_DISTANCE < z_dist)
		{
			if(it->second.GetActor())
				m_scene.RemoveActor(*it->second.GetActor());
			if(it->second.GetWaterActor())
				m_scene.RemoveActor(*it->second.GetWaterActor());
			it = m_chunks.erase(it);
		}
		else
			++it;
	}
}

#define QUIT_CHECK() if(m_generation_status == GenerationState::Quitting) goto quit
void World::GenerateWorld()
{
	for(;;)
	{
		QUIT_CHECK();
		if(m_generation_status != GenerationState::Working)
		{
			QUIT_CHECK();
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(16ms);
			continue;
		}
		std::queue<std::reference_wrapper<Chunk>> mesh_generation_queue;

		{
			Scop::Vec2i x_range{ m_current_chunk_position.x - RENDER_DISTANCE - 1, m_current_chunk_position.x + RENDER_DISTANCE + 1 };
			Scop::Vec2i z_range{ m_current_chunk_position.y - RENDER_DISTANCE - 1, m_current_chunk_position.y + RENDER_DISTANCE + 1 };
			std::size_t range = (RENDER_DISTANCE + RENDER_DISTANCE + 2) * 2;

			float progress = 0.0f;
			m_loading_progress = 0;

			float i = 0;
			for(std::int32_t x = x_range.x; x <= x_range.y; x++, i++)
			{
				for(std::int32_t z = z_range.x; z <= z_range.y; z++)
				{
					QUIT_CHECK();
					auto res = m_chunks.try_emplace(Scop::Vec2i{ x, z }, *this, Scop::Vec2i{ x, z });
					res.first->second.GenerateChunk();

					progress = (i / range) * 30.0f;
					m_loading_progress = progress;

					if(!res.first->second.GetActor() && x > x_range.x && x < x_range.y && z > z_range.x && z < z_range.y)
						mesh_generation_queue.push(std::ref(res.first->second));
				}
			}
		}

		{
			float progress = 30.0f;
			m_loading_progress = 30;

			std::size_t range = mesh_generation_queue.size() - 30;

			for(float i = 0.0f; !mesh_generation_queue.empty(); i++)
			{
				QUIT_CHECK();
				auto chunk = mesh_generation_queue.front();
				mesh_generation_queue.pop();
				chunk.get().GenerateMesh();
				m_chunks_to_upload.Push(chunk);

				progress = ((i - 30.0f) / range) * 70;
				m_loading_progress = progress + 30;
			}
		}

		m_loading_progress = 100;
		m_generation_status = GenerationState::Finished;
	}

quit:
	m_generation_status = GenerationState::Finished;
}

void World::Upload()
{
	if(m_chunks_to_upload.IsEmpty())
		return;
	Scop::RenderCore::Get().ShouldStackSubmits(true);
	for(std::size_t i = 0; i < CHUNKS_UPLOAD_PER_FRAME && !m_chunks_to_upload.IsEmpty(); i++)
	{
		auto chunk = m_chunks_to_upload.Pop();
		chunk.get().UploadMesh();
	}
	Scop::RenderCore::Get().WaitQueueIdle(KVF_GRAPHICS_QUEUE);
	Scop::RenderCore::Get().ShouldStackSubmits(false);
}

void World::SetupLoading()
{
	Scop::CPUBuffer default_pixels{ sizeof(std::uint32_t) };
	default_pixels.GetDataAs<std::uint32_t>()[0] = 0xFFFFFFFF;
	Scop::Sprite& progress_bar = m_scene.CreateSprite(std::make_shared<Scop::Texture>(std::move(default_pixels), 1, 1));

	Scop::Text& loading_text = m_scene.CreateText("Loading...");

	Scop::Vec2ui32 loading_size;
	Scop::Sprite& loading = m_scene.CreateSprite(std::make_shared<Scop::Texture>(Scop::LoadBMPFile(GetResourcesPath() / "loading.bmp", loading_size), loading_size.x, loading_size.y));

	auto loading_update = [this, loading_size, &progress_bar, &loading_text](Scop::NonOwningPtr<Scop::Scene> scene, Scop::NonOwningPtr<Scop::Sprite> sprite, Scop::Inputs& input, float delta)
	{
		static bool just_ended_loading = false;
		static std::uint32_t last_percentage = 0;

		if(!m_show_loading_screen || Scop::CommandLineInterface::Get().HasFlag("no-loading-screen"))
		{
			if(!just_ended_loading)
			{
				sprite->SetColor(Scop::Vec4f{ 0.0f });
				progress_bar.SetColor(Scop::Vec4f{ 0.0f });
				reinterpret_cast<Scop::FirstPerson3D*>(m_scene.GetCamera().get())->EnableCamera();
				if(p_loading_text)
					m_scene.RemoveText(*p_loading_text);
				m_scene.RemoveText(loading_text);
				m_scene.RemoveSprite(progress_bar);
				just_ended_loading = true;
			}
			return;
		}
		reinterpret_cast<Scop::FirstPerson3D*>(m_scene.GetCamera().get())->DisableCamera();
		Scop::Vec2f scale = Scop::Vec2f{
			static_cast<float>(Scop::ScopEngine::Get().GetWindow().GetWidth()) / static_cast<float>(loading_size.x),
			static_cast<float>(Scop::ScopEngine::Get().GetWindow().GetHeight()) / static_cast<float>(loading_size.y),
		};
		sprite->SetScale(scale);
		sprite->SetPosition(Scop::Vec2ui{ 0, 0 });

		if(last_percentage != m_loading_progress)
		{
			last_percentage = m_loading_progress;
			if(p_loading_text)
				m_scene.RemoveText(*p_loading_text);
			p_loading_text = &m_scene.CreateText(std::to_string(m_loading_progress) + '%');
		}
		if(p_loading_text)
			p_loading_text->SetPosition(Scop::Vec2ui{ (Scop::ScopEngine::Get().GetWindow().GetWidth() >> 1) + 40, (Scop::ScopEngine::Get().GetWindow().GetHeight() >> 1) - 55 });
		loading_text.SetPosition(Scop::Vec2ui{ (Scop::ScopEngine::Get().GetWindow().GetWidth() >> 1) - 40, (Scop::ScopEngine::Get().GetWindow().GetHeight() >> 1) - 55 });

		Scop::Vec2ui progress_size = Scop::Vec2ui{
			static_cast<std::uint32_t>(static_cast<float>(m_loading_progress) * 4.0f),
			25
		};
		progress_bar.SetScale(Scop::Vec2f(progress_size));
		progress_bar.SetPosition(Scop::Vec2ui{ (Scop::ScopEngine::Get().GetWindow().GetWidth() >> 1) - 200, (Scop::ScopEngine::Get().GetWindow().GetHeight() >> 1) - (progress_size.y >> 1) + 25 });
	};

	using sprite_hook = std::function<void(Scop::NonOwningPtr<Scop::Sprite>)>;
	loading.AttachScript(std::make_shared<Scop::NativeSpriteScript>(sprite_hook{}, loading_update, sprite_hook{}));
}
