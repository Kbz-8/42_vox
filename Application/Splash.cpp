#include <ScopCore.h>
#include <ScopGraphics.h>
#include <Utils.h>
#include <thread>

Scop::Scene& SplashScreen()
{
	Scop::SceneDescriptor scene_desc;
	scene_desc.fragment_shader = Scop::RenderCore::Get().GetDefaultFragmentShader();
	scene_desc.render_3D_enabled = false;
	scene_desc.render_skybox_enabled = false;
	Scop::Scene& scene = Scop::ScopEngine::Get().CreateMainScene("splash", std::move(scene_desc));

	Scop::Vec2ui32 splash_size;
	Scop::Sprite& splash = scene.CreateSprite(std::make_shared<Scop::Texture>(Scop::LoadBMPFile(Scop::ScopEngine::Get().GetAssetsPath() / "Images/splashscreen.bmp", splash_size), splash_size.x, splash_size.y));
	splash.SetPosition(Scop::Vec2ui{ Scop::ScopEngine::Get().GetWindow().GetWidth() / 2 - splash_size.x / 2, Scop::ScopEngine::Get().GetWindow().GetHeight() / 2 - splash_size.y / 2 });

	scene.LoadFont(GetResourcesPath() / "Font.ttf", 16.0f);
	Scop::Text& copyright_text = scene.CreateText("Copyright maldavid Studios");
	copyright_text.SetScale(Scop::Vec2f{ 0.75f });

	auto splash_update = [splash_size, &copyright_text](Scop::NonOwningPtr<Scop::Scene> scene, Scop::NonOwningPtr<Scop::Sprite> sprite, Scop::Inputs& input, float delta)
	{
		using namespace std::chrono_literals;

		if(Scop::CommandLineInterface::Get().HasFlag("skip-splash"))
			scene->SwitchToChild("main");

		static float x = 0.02f;
		Scop::Vec4f color = sprite->GetColor();
		color.w = std::abs(std::sin(x)) * 1.1;
		x += 0.02f;
		sprite->SetColor(color); 
		sprite->SetPosition(Scop::Vec2ui{ (Scop::ScopEngine::Get().GetWindow().GetWidth() >> 1) - (splash_size.x >> 1), (Scop::ScopEngine::Get().GetWindow().GetHeight() >> 1) - (splash_size.y >> 1) });
		copyright_text.SetPosition(Scop::Vec2ui{ Scop::ScopEngine::Get().GetWindow().GetWidth() - 175, Scop::ScopEngine::Get().GetWindow().GetHeight() - 20 });
		if(color.w <= 0.02f)
			scene->SwitchToChild("main");

		std::this_thread::sleep_for(33ms); // 30fps
	};

	using sprite_hook = std::function<void(Scop::NonOwningPtr<Scop::Sprite>)>;
	splash.AttachScript(std::make_shared<Scop::NativeSpriteScript>(sprite_hook{}, splash_update, sprite_hook{}));

	return scene;
}
