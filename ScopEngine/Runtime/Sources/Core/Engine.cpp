#include <Core/Engine.h>
#include <Renderer/RenderCore.h>
#include <SDL2/SDL.h>
#include <Core/Logs.h>
#include <Core/EventBus.h>
#include <csignal>

namespace Scop
{
	namespace Internal
	{
		struct InterruptEvent : public EventBase
		{
			Event What() const override { return Event::QuitEventCode; }
		};

		struct SceneChangedEvent : public EventBase
		{
			Event What() const override { return Event::SceneHasChangedEventCode; }
		};
	}

	void FatalErrorEventHandle()
	{
		std::abort();
	}

	void SignalHandler([[maybe_unused]] int sig)
	{
		EventBus::Send("__ScopEngine", Internal::InterruptEvent{});
	}

	ScopEngine* ScopEngine::s_instance = nullptr;

	ScopEngine::ScopEngine(int ac, char** av, const std::string& title, std::uint32_t width, std::uint32_t height, std::filesystem::path assets_path)
		: m_renderer(), m_window(title, width, height), m_assets_path(std::move(assets_path))
		#ifdef DEBUG
			,m_imgui(&m_renderer)
		#endif
	{
		s_instance = this;
		std::function<void(const EventBase&)> functor = [this](const EventBase& event)
		{
			if(event.What() == Event::FatalErrorEventCode)
				FatalErrorEventHandle();
			if(event.What() == Event::QuitEventCode)
				this->Quit();
		};
		EventBus::RegisterListener({ functor, "__ScopEngine" });

		m_cli.Feed(ac, av);

		signal(SIGINT, SignalHandler);

		SDL_SetHint("SDL_MOUSE_RELATIVE_MODE_WARP", "1");
		SDL_SetHint("SDL_MOUSE_RELATIVE_MODE_CENTER", "1");
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
			FatalError("SDL error: unable to init all subsystems : %", SDL_GetError());
		p_renderer_core = std::make_unique<RenderCore>();
		m_renderer.Init(&m_window);
		m_scene_renderer.Init();
		#ifdef DEBUG
			m_imgui.Init(m_inputs);
		#endif
	}

	ScopEngine& ScopEngine::Get() noexcept
	{
		Verify(s_instance != nullptr, "ScopEngine has not been instanciated");
		return *s_instance;
	}

	void ScopEngine::Run()
	{
		Verify(p_current_scene, "no main scene registered");
		float old_timestep = static_cast<float>(SDL_GetTicks64()) / 1000.0f;
		p_current_scene->Init(&m_renderer);
		while(m_running)
		{
			float current_timestep = (static_cast<float>(SDL_GetTicks64()) / 1000.0f) - old_timestep;
			old_timestep = static_cast<float>(SDL_GetTicks64()) / 1000.0f;

			m_inputs.Update();
			m_window.FetchWindowInfos();
			p_current_scene->Update(m_inputs, current_timestep, static_cast<float>(m_window.GetWidth()) / static_cast<float>(m_window.GetHeight()));

			if(m_scene_changed)
			{
				RenderCore::Get().WaitDeviceIdle();
				EventBus::SendBroadcast(Internal::SceneChangedEvent{});
				m_scene_changed = false;
				continue;
			}

			m_renderer.BeginFrame();
				m_scene_renderer.Render(*p_current_scene, m_renderer);
				#ifdef DEBUG
					m_imgui.BeginFrame();
					m_imgui.DisplayRenderStatistics();
					m_imgui.EndFrame();
				#endif
			m_renderer.EndFrame();

			if(m_running)
				m_running = !m_inputs.HasRecievedCloseEvent();
		}
	}

	ScopEngine::~ScopEngine()
	{
		RenderCore::Get().WaitDeviceIdle();
		p_main_scene->Destroy();
		p_main_scene.reset();
		m_window.Destroy();
		#ifdef DEBUG
			m_imgui.Destroy();
		#endif
		m_scene_renderer.Destroy();
		m_renderer.Destroy();
		Model::s_default_material.reset();
		p_renderer_core.reset();
		SDL_Quit();
		Message("Successfully executed !");
		s_instance = nullptr;
	}
}
