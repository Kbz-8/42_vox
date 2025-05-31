#include <thread>

#include <ScopCore.h>

#include <Block.h>
#include <World.h>
#include <Utils.h>

World::World(Scop::Scene& scene) : m_noisecollection(42), m_scene(scene), m_previous_chunk_position(-1000, 10000)
{
	Scop::Vec2ui32 map_size;
	Scop::MaterialTextures material_params;
	material_params.albedo = std::make_shared<Scop::Texture>(Scop::LoadBMPFile(GetResourcesPath() / "atlas.bmp", map_size), map_size.x, map_size.y);
	p_block_material = std::make_shared<Scop::Material>(material_params);

	std::thread(&World::GenerateWorld, this).detach();

	auto narrator_update = [this](Scop::NonOwningPtr<Scop::Scene> scene, Scop::Inputs& input, float delta)
	{
		static bool generate = true;
		static bool generation_debounce = false;
		static bool wireframe_debounce = false;

		Scop::FirstPerson3D* camera = reinterpret_cast<Scop::FirstPerson3D*>(m_scene.GetCamera().get());
		std::int32_t x_chunk = static_cast<std::int32_t>(camera->GetPosition().x) / static_cast<std::int32_t>(CHUNK_SIZE.x);
		std::int32_t z_chunk = static_cast<std::int32_t>(camera->GetPosition().z) / static_cast<std::int32_t>(CHUNK_SIZE.z);
		m_current_chunk_position = Scop::Vec2i{ x_chunk, z_chunk };

		if(Scop::NonOwningPtr<Chunk> current_chunk = GetChunk(m_current_chunk_position); current_chunk)
		{
			Scop::Vec3i camera_pos = Scop::Vec3i(std::floor(scene->GetCamera()->GetPosition().x), scene->GetCamera()->GetPosition().y, std::floor(scene->GetCamera()->GetPosition().z));
			camera_pos.x %= CHUNK_SIZE.x;
			camera_pos.z %= CHUNK_SIZE.z;
			camera_pos.x += CHUNK_SIZE.x;
			camera_pos.z += CHUNK_SIZE.z;
			scene->GetPostProcessData().data.GetDataAs<std::int32_t>()[0] = current_chunk->GetBlock(Scop::Vec3i(camera_pos.x % CHUNK_SIZE.x, camera_pos.y, camera_pos.z % CHUNK_SIZE.z)) == static_cast<std::uint32_t>(BlockType::Water);
		}

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
					m_generation_status = GenerationState::Ready;
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

		for(std::int32_t x = m_current_chunk_position.x - RENDER_DISTANCE; x <= m_current_chunk_position.x + RENDER_DISTANCE; x++)
		{
			for(std::int32_t z = m_current_chunk_position.y - RENDER_DISTANCE; z <= m_current_chunk_position.y + RENDER_DISTANCE; z++)
			{
				QUIT_CHECK();
				auto res = m_chunks.try_emplace(Scop::Vec2i{ x, z }, *this, Scop::Vec2i{ x, z });
				if(res.second)
				{
					if(!res.first->second.GetActor())
					{
						res.first->second.GenerateChunk();
						mesh_generation_queue.push(std::ref(res.first->second));
					}
				}
			}
		}
		while(!mesh_generation_queue.empty())
		{
			QUIT_CHECK();
			auto chunk = mesh_generation_queue.front();
			mesh_generation_queue.pop();
			chunk.get().GenerateMesh();
			m_chunks_to_upload.Push(chunk);
		}
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
