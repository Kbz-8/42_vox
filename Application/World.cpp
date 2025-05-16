#include <ScopCore.h>

#include <World.h>
#include <Utils.h>

World::World(Scop::Scene& scene) : m_scene(scene), m_narrator(scene.CreateNarrator()), m_previous_chunk_position(-1000, 10000)
{
	Scop::Vec2ui32 map_size;
	Scop::MaterialTextures material_params;
	material_params.albedo = std::make_shared<Scop::Texture>(Scop::LoadBMPFile(GetResourcesPath() / "prototype.bmp", map_size), map_size.x, map_size.y);
	p_block_material = std::make_shared<Scop::Material>(material_params);

	auto narrator_update = [this](Scop::NonOwningPtr<Scop::Scene> scene, Scop::Inputs& input, float delta)
	{
		GenerateWorld();
		Upload();
	};

	m_narrator.AttachScript(std::make_shared<Scop::NativeNarratorScript>(std::function<void()>{}, narrator_update, std::function<void()>{}));
}

[[nodiscard]] Scop::NonOwningPtr<Chunk> World::GetChunk(Scop::Vec2i position)
{
	auto it = m_chunks.find(position);
	if(it == m_chunks.end())
		return nullptr;
	return &it->second;
}

void World::GenerateWorld()
{
	Scop::FirstPerson3D* camera = reinterpret_cast<Scop::FirstPerson3D*>(m_scene.GetCamera().get());
	std::int32_t x_chunk = static_cast<std::int32_t>(camera->GetPosition().x) / static_cast<std::int32_t>(CHUNK_SIZE.x);
	std::int32_t z_chunk = static_cast<std::int32_t>(camera->GetPosition().z) / static_cast<std::int32_t>(CHUNK_SIZE.z);
	Scop::Vec2i current_chunk_position{ x_chunk, z_chunk };

	if(current_chunk_position == m_previous_chunk_position)
		return;

	for(auto it = m_chunks.begin(); it != m_chunks.end();)
	{
		Scop::Vec3i pos = it->first;

		float x_dist = std::abs(pos.x - current_chunk_position.x);
		float z_dist = std::abs(pos.z - current_chunk_position.y);
		if(RENDER_DISTANCE < x_dist || RENDER_DISTANCE < z_dist)
		{
			if(it->second.GetActor())
				m_scene.RemoveActor(*it->second.GetActor());
			it = m_chunks.erase(it);
		}
		else
			++it;
	}

	for(std::int32_t x = x_chunk - RENDER_DISTANCE; x <= x_chunk + RENDER_DISTANCE; x++)
	{
		for(std::int32_t z = z_chunk - RENDER_DISTANCE; z <= z_chunk + RENDER_DISTANCE; z++)
		{
			auto res = m_chunks.try_emplace(Scop::Vec2i{ x, z }, *this, Scop::Vec2i{ x, z });
			if(res.second)
				res.first->second.GenerateChunk();
		}
	}

	for(auto& chunk : m_chunks)
		chunk.second.GenerateMesh();

	m_previous_chunk_position = current_chunk_position;
}

void World::Upload()
{
	for(auto& chunk : m_chunks)
		chunk.second.UploadMesh();
}
