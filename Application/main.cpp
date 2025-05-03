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

	engine.RegisterMainScene(splash_scene.get());
	engine.Run();
	return 0;
}
