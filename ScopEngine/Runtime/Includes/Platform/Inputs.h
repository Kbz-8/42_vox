#ifndef __SCOP_PLATFORM_INPUTS__
#define __SCOP_PLATFORM_INPUTS__

#include <array>
#include <vector>
#include <cstdint>
#include <functional>

#include <SDL2/SDL.h>

namespace Scop
{
	using EventUpdateHook = std::function<void(SDL_Event*)>;

	class Inputs
	{
		friend class ScopEngine;

		public:
			Inputs();

			[[nodiscard]] inline bool IsKeyPressed(const std::uint32_t button) const noexcept { return m_keys[button]; }
			[[nodiscard]] inline bool IsMouseButtonPressed(const std::uint8_t button) const noexcept { return m_mouse[button - 1]; }
			[[nodiscard]] inline bool IsMouseWheelUp() const noexcept { return m_is_mouse_wheel_up; }
			[[nodiscard]] inline bool IsMouseWheelDown() const noexcept { return m_is_mouse_wheel_down; }
			[[nodiscard]] inline std::int32_t GetX() const noexcept { return m_x; }
			[[nodiscard]] inline std::int32_t GetY() const noexcept { return m_y; }
			[[nodiscard]] inline std::int32_t GetXRel() const noexcept { return m_x_rel; }
			[[nodiscard]] inline std::int32_t GetYRel() const noexcept { return m_y_rel; }

			inline void AddEventUpdateHook(const EventUpdateHook& hook) { m_hooks.push_back(hook); }

			inline void GrabMouse() noexcept { SDL_SetRelativeMouseMode(SDL_TRUE); SDL_ShowCursor(SDL_DISABLE); m_is_mouse_grabbed = true; }
			inline void ReleaseMouse() noexcept { SDL_SetRelativeMouseMode(SDL_FALSE); SDL_ShowCursor(SDL_ENABLE); m_is_mouse_grabbed = false; }
			[[nodiscard]] inline bool IsMouseGrabbed() const noexcept { return m_is_mouse_grabbed; }

			[[nodiscard]] inline bool HasRecievedCloseEvent() const noexcept { return m_has_recieved_close_event; }

			~Inputs() = default;

		private:
			void Update();

		private:
			SDL_Event m_event;
			std::vector<EventUpdateHook> m_hooks;
			const std::uint8_t* m_keys;
			std::array<bool, 5> m_mouse; // 5 bytes, shitty padding, maybe fix
			std::int32_t m_keys_count = 0;
			std::int32_t m_x = 0;
			std::int32_t m_y = 0;
			std::int32_t m_x_rel = 0;
			std::int32_t m_y_rel = 0;
			bool m_has_recieved_close_event = false;
			bool m_is_mouse_grabbed = false;
			bool m_is_mouse_wheel_up = false;
			bool m_is_mouse_wheel_down = false;
	};
}

#endif
