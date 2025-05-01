#include <Platform/Window.h>
#include <SDL2/SDL_vulkan.h>
#include <Core/Logs.h>

namespace Scop
{
	Window::Window(const std::string& title, std::uint32_t width, std::uint32_t height, bool hidden) : m_title(title), m_height(height), m_width(width)
	{
		std::uint32_t flags = SDL_WINDOW_VULKAN;
		if(hidden)
			flags |= SDL_WINDOW_HIDDEN;
		else
			flags |= SDL_WINDOW_SHOWN;

		if(width == 0 && height == 0)
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		else
			flags |= SDL_WINDOW_RESIZABLE;

		p_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
		if(!p_window)
			FatalError("Unable to open a new window, %", SDL_GetError());
	}

	void Window::FetchWindowInfos() noexcept
	{
		Vec2i size;
		SDL_GetWindowSize(p_window, &size.x, &size.y);
		m_width = size.x;
		m_height = size.y;
	}

	VkSurfaceKHR Window::CreateVulkanSurface(VkInstance instance) const noexcept
	{
		VkSurfaceKHR surface;
		if(!SDL_Vulkan_CreateSurface(p_window, instance, &surface))
			FatalError("SDL: could not create a Vulkan surface; %", SDL_GetError());
		return surface;
	}

	std::vector<const char*> Window::GetRequiredVulkanInstanceExtentions() const noexcept
	{
		std::uint32_t count;
		if(!SDL_Vulkan_GetInstanceExtensions(p_window, &count, nullptr))
			FatalError("SDL Manager: could not retrieve Vulkan instance extensions");
		std::vector<const char*> extensions(count);
		if(!SDL_Vulkan_GetInstanceExtensions(p_window, &count, extensions.data()))
			FatalError("SDL Manager: could not retrieve Vulkan instance extensions");
		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		return extensions;
	}

	Vec2ui Window::GetVulkanDrawableSize() const noexcept
	{
		Vec2i extent;
		SDL_Vulkan_GetDrawableSize(p_window, &extent.x, &extent.y);
		return Vec2ui{ extent };
	}

	void Window::Destroy() noexcept
	{
		if(p_window != nullptr)
		{
			SDL_DestroyWindow(p_window);
			p_window = nullptr;
		}
	}

	Window::~Window()
	{
		Destroy();
	}
}
