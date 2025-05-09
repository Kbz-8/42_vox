#include <ScopCore.h>

#include <World.h>
#include <Utils.h>

World::World(Scop::Scene& scene) : m_scene(scene), m_narrator(scene.CreateNarrator())
{
	Scop::Vec2ui32 map_size;
	Scop::MaterialTextures material_params;
	material_params.albedo = std::make_shared<Scop::Texture>(Scop::LoadBMPFile(GetResourcesPath() / "prototype.bmp", map_size), map_size.x, map_size.y);
	p_block_material = std::make_shared<Scop::Material>(material_params);

	auto narrator_update = [this](Scop::NonOwningPtr<Scop::Scene> scene, Scop::Inputs& input, float delta)
	{
		if(m_chunks.empty())
			GenerateWorld();
	};

	m_narrator.AttachScript(std::make_shared<Scop::NativeNarratorScript>(std::function<void()>{}, narrator_update, std::function<void()>{}));
}

[[nodiscard]] Chunk& World::GetChunk(Scop::Vec2i position)
{
}

void World::GenerateWorld()
{
	for(std::int32_t x = 0; x < 4; x++)
	{
		for(std::int32_t z = 0; z < 4; z++)
		{
			m_chunks.emplace_back(*this, Scop::Vec2i{ x, z });
			m_chunks.back().GenerateChunk();
		}
	}
}
