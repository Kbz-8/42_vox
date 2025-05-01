#include <Platform/Inputs.h>
#include <Core/EventBus.h>

namespace Scop
{
	namespace Internal
	{
		struct ResizeEventSwapchain : public EventBase
		{
			Event What() const override { return Event::ResizeEventCode; }
		};
	}
	
	Inputs::Inputs() : m_keys(SDL_GetKeyboardState(&m_keys_count))
	{}

	void Inputs::Update()
	{
		SDL_GetRelativeMouseState(&m_x_rel, &m_y_rel);
		std::uint8_t mouse_state = SDL_GetMouseState(&m_x, &m_y);
		m_mouse[0] = SDL_BUTTON(mouse_state) & SDL_BUTTON_LMASK;
		m_mouse[1] = SDL_BUTTON(mouse_state) & SDL_BUTTON_MMASK;
		m_mouse[2] = SDL_BUTTON(mouse_state) & SDL_BUTTON_RMASK;
		m_mouse[3] = SDL_BUTTON(mouse_state) & SDL_BUTTON_X1MASK;
		m_mouse[4] = SDL_BUTTON(mouse_state) & SDL_BUTTON_X2MASK;

		m_is_mouse_wheel_up = false;
		m_is_mouse_wheel_down = false;

		while(SDL_PollEvent(&m_event))
		{
			for(auto& hook : m_hooks)
				hook(&m_event);
			switch(m_event.type)
			{
				case SDL_MOUSEWHEEL:
				{
					if(m_event.wheel.y > 0) // scroll up
						m_is_mouse_wheel_up = true;
					else if(m_event.wheel.y < 0) // scroll down
						m_is_mouse_wheel_down = true;
					break;
				}

				case SDL_WINDOWEVENT:
				{
					switch(m_event.window.event)
					{
						case SDL_WINDOWEVENT_CLOSE: m_has_recieved_close_event = true; break;
						case SDL_WINDOWEVENT_SIZE_CHANGED: EventBus::Send("__ScopSwapchain", Internal::ResizeEventSwapchain{}); break;
						default: break;
					}
					break;
				}
				default: break;
			}
		}
	}
}
