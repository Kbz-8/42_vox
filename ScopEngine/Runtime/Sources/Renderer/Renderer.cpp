#include <Renderer/Renderer.h>
#include <Core/Logs.h>
#include <Core/Enums.h>
#include <Core/Engine.h>
#include <Core/EventBus.h>

namespace Scop
{
	namespace Internal
	{
		struct FrameBeginEventBroadcast : public EventBase
		{
			Event What() const override { return Event::FrameBeginEventCode; }
		};
	}

	void Renderer::Init(NonOwningPtr<Window> window)
	{
		std::function<void(const EventBase&)> functor = [this](const EventBase& event)
		{
			if(event.What() == Event::ResizeEventCode)
			{
				for(std::size_t i = 0; i < m_render_finished_semaphores.size(); i++)
				{
					kvfDestroySemaphore(RenderCore::Get().GetDevice(), m_render_finished_semaphores[i]);
					Message("Vulkan: render finished semaphore destroyed");
				}
				m_render_finished_semaphores.clear();
				for(std::size_t i = 0; i < m_swapchain.GetImagesCount(); i++)
				{
					m_render_finished_semaphores.push_back(kvfCreateSemaphore(RenderCore::Get().GetDevice()));
					Message("Vulkan: render finished semaphore created");
				}
			}
		};

		EventBus::RegisterListener({ functor, "__ScopRenderer" });
		p_window = window;
		m_swapchain.Init(p_window);
		for(std::size_t i = 0; i < m_swapchain.GetImagesCount(); i++)
		{
			m_render_finished_semaphores.push_back(kvfCreateSemaphore(RenderCore::Get().GetDevice()));
			Message("Vulkan: render finished semaphore created");
		}
		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_cmd_buffers[i] = kvfCreateCommandBuffer(RenderCore::Get().GetDevice());
			Message("Vulkan: command buffer created");
			m_cmd_fences[i] = kvfCreateFence(RenderCore::Get().GetDevice());
			Message("Vulkan: fence created");
			m_image_available_semaphores[i] = kvfCreateSemaphore(RenderCore::Get().GetDevice());
			Message("Vulkan: image available semaphore created");
		}
	}

	void Renderer::BeginFrame()
	{
		kvfWaitForFence(RenderCore::Get().GetDevice(), m_cmd_fences[m_current_frame_index]);
		m_swapchain.AquireFrame(m_image_available_semaphores[m_current_frame_index]);
		RenderCore::Get().vkResetCommandBuffer(m_cmd_buffers[m_current_frame_index], 0);
		kvfBeginCommandBuffer(m_cmd_buffers[m_current_frame_index], 0);
		m_drawcalls = 0;
		m_polygons_drawn = 0;
		EventBus::SendBroadcast(Internal::FrameBeginEventBroadcast{});
	}

	void Renderer::EndFrame()
	{
		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		kvfEndCommandBuffer(m_cmd_buffers[m_current_frame_index]);
		kvfSubmitCommandBuffer(RenderCore::Get().GetDevice(), m_cmd_buffers[m_current_frame_index], KVF_GRAPHICS_QUEUE, m_render_finished_semaphores[m_swapchain.GetImageIndex()], m_image_available_semaphores[m_current_frame_index], m_cmd_fences[m_current_frame_index], wait_stages);
		m_swapchain.Present(m_render_finished_semaphores[m_swapchain.GetImageIndex()]);
		m_current_frame_index = (m_current_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::Destroy() noexcept
	{
		RenderCore::Get().WaitDeviceIdle();
		for(std::size_t i = 0; i < m_render_finished_semaphores.size(); i++)
		{
			kvfDestroySemaphore(RenderCore::Get().GetDevice(), m_render_finished_semaphores[i]);
			Message("Vulkan: render finished semaphore destroyed");
		}
		m_render_finished_semaphores.clear();
		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			kvfDestroySemaphore(RenderCore::Get().GetDevice(), m_image_available_semaphores[i]);
			Message("Vulkan: image available semaphore destroyed");
			kvfDestroyCommandBuffer(RenderCore::Get().GetDevice(), m_cmd_buffers[i]);
			Message("Vulkan: command buffer destroyed");
			kvfDestroyFence(RenderCore::Get().GetDevice(), m_cmd_fences[i]);
			Message("Vulkan: fence destroyed");
		}
		m_swapchain.Destroy();
	}
}
