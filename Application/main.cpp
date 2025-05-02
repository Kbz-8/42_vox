#include <ScopCore.h>
#include <ScopGraphics.h>

#include <ScriptSubRoutines.h>
#include <Splash.h>

#include <climits>
#include <memory>
#include <unistd.h>
#include <filesystem>
#include <string>
#include <cmath>

std::filesystem::path GetExecutablePath()
{
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
}

std::filesystem::path GetResourcesPath()
{
	return GetExecutablePath().parent_path().parent_path() / "Resources";
}

int main(int ac, char** av)
{
	Scop::ScopEngine engine(ac, av, "Vox", 0, 0, GetExecutablePath().parent_path().parent_path() / "ScopEngine/Assets");

	std::shared_ptr<Scop::Scene> splash_scene = SplashScreen();

	Scop::SceneDescriptor main_scene_desc;
	main_scene_desc.fragment_shader = Scop::RenderCore::Get().GetDefaultFragmentShader();
	main_scene_desc.camera = std::make_shared<Scop::FirstPerson3D>(Scop::Vec3f{ -10.0f, 0.0f, 0.0f }, 80.f);
	Scop::Scene& main_scene = splash_scene->AddChildScene("main", std::move(main_scene_desc));

	Scop::Vec2ui32 skybox_size;
	main_scene.AddSkybox(std::make_shared<Scop::CubeTexture>(Scop::LoadBMPFile(GetResourcesPath() / "skybox.bmp", skybox_size), skybox_size.x, skybox_size.y));

	Scop::Actor& object = main_scene.CreateActor(Scop::LoadModelFromObjFile(GetResourcesPath() / "knuckles.obj"));
	object.SetScale(Scop::Vec3f{ 5.0f, 5.0f, 5.0f });

	Scop::Actor& object2 = main_scene.CreateActor(Scop::CreateSphere());
	object2.SetScale(Scop::Vec3f{ 5.0f, 5.0f, 5.0f });
	object2.SetPosition(Scop::Vec3f{ 10.0f, 10.0f, 10.0f });

	Scop::Vec2ui32 map_size;

	Scop::MaterialTextures material_params;
	material_params.albedo = std::make_shared<Scop::Texture>(Scop::LoadBMPFile(GetResourcesPath() / "knuckles.bmp", map_size), map_size.x, map_size.y);

	std::shared_ptr<Scop::Material> material = std::make_shared<Scop::Material>(material_params);
	object.GetModelRef().SetMaterial(material, 0);

	auto object_update = [](Scop::NonOwningPtr<Scop::Scene> scene, Scop::NonOwningPtr<Scop::Actor> actor, Scop::Inputs& input, float delta)
	{
		static Scop::MaterialData material_data{};

		WireframeHandler(scene, input);
		MovementHandler(actor, input, delta);
		ColorsTransitionHandler(actor, input, delta, material_data);
		TextureTransitionHandler(actor, input, delta, material_data);

		for(std::shared_ptr<Scop::Material> material : actor->GetModelRef().GetAllMaterials())
		{
			if(material)
				material->SetMaterialData(material_data);
		}
	};

	using actor_hook = std::function<void(Scop::NonOwningPtr<Scop::Actor>)>;
	object.AttachScript(std::make_shared<Scop::NativeActorScript>(actor_hook{}, object_update, actor_hook{}));

	engine.RegisterMainScene(splash_scene.get());
	engine.Run();
	return 0;
}
