#include <ScopCore.h>

#include <World.h>
#include <Utils.h>

World::World(Scop::Scene& scene) : m_scene(scene), m_narrator(scene.CreateNarrator())
{
	Scop::Vec2ui32 map_size;
	Scop::MaterialTextures material_params;
	material_params.albedo = std::make_shared<Scop::Texture>(Scop::LoadBMPFile(GetResourcesPath() / "prototype.bmp", map_size), map_size.x, map_size.y);
	p_block_material = std::make_shared<Scop::Material>(material_params);

	auto narrator_update = [](Scop::NonOwningPtr<Scop::Scene> scene, Scop::Inputs& input, float delta)
	{
	};

	m_narrator.AttachScript(std::make_shared<Scop::NativeNarratorScript>(std::function<void()>{}, narrator_update, std::function<void()>{}));
}
