#ifndef __SCOP_CORE_ENGINE__
#define __SCOP_CORE_ENGINE__

#include <cstdint>
#include <filesystem>

#include <Platform/Window.h>
#include <Platform/Inputs.h>
#include <Renderer/Renderer.h>
#include <Renderer/ScenesRenderer.h>
#include <Renderer/RenderCore.h>
#include <Core/Logs.h>
#include <Graphics/Scene.h>
#include <Core/CLI.h>

#ifdef DEBUG
	#include <Debug/ImGuiRenderer.h>
#endif

namespace Scop
{
	class ScopEngine
	{
		friend class Scene;

		public:
			ScopEngine(int ac, char** av, const std::string& title, std::uint32_t width, std::uint32_t height, std::filesystem::path assets_path);

			void Run();

			[[nodiscard]] inline const Window& GetWindow() const noexcept { return m_window; }
			[[nodiscard]] inline std::filesystem::path GetAssetsPath() const { return m_assets_path; }

			inline Scene& CreateMainScene(std::string_view name, SceneDescriptor desc) noexcept { p_main_scene = std::make_unique<Scene>(name, std::move(desc)); p_current_scene = p_main_scene.get(); return *p_main_scene; }
			inline NonOwningPtr<Scene> GetRootScene() const noexcept { return p_main_scene.get(); }

			constexpr void Quit() noexcept { m_running = false; }

			[[nodiscard]] static inline bool IsInit() noexcept { return s_instance != nullptr; }
			[[nodiscard]] static ScopEngine& Get() noexcept;

			~ScopEngine();

		private:
			inline void SwitchToScene(NonOwningPtr<Scene> current) noexcept { p_current_scene = current; m_scene_changed = true; }

		private:
			static ScopEngine* s_instance;

			Inputs m_inputs;
			Renderer m_renderer;
			#ifdef DEBUG
				ImGuiRenderer m_imgui;
			#endif
			CommandLineInterface m_cli;
			Window m_window;
			SceneRenderer m_scene_renderer;
			std::filesystem::path m_assets_path;
			std::unique_ptr<RenderCore> p_renderer_core;
			std::unique_ptr<Scene> p_main_scene;
			NonOwningPtr<Scene> p_current_scene;
			bool m_running = true;
			bool m_scene_changed = false;
	};
}

#endif
