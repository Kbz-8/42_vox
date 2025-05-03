#include <Renderer/RenderPasses/Passes.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>

namespace Scop
{
	void RenderPasses::Init()
	{
		m_skybox.Init();
		m_2Dpass.Init();
		m_final.Init();

		std::function<void(const EventBase&)> functor = [this](const EventBase& event)
		{
			if(event.What() == Event::ResizeEventCode)
				m_main_render_texture.Destroy();
		};
		EventBus::RegisterListener({ functor, "__ScopRenderPasses" });
	}

	void RenderPasses::Pass(Scene& scene, Renderer& renderer)
	{
		if(!m_main_render_texture.IsInit())
		{
			auto extent = kvfGetSwapchainImagesSize(renderer.GetSwapchain().Get());
			m_main_render_texture.Init({}, extent.width, extent.height, VK_FORMAT_R8G8B8A8_UNORM, false, "scop_main_render_texture");
		}

		scene.GetDepth().Clear(renderer.GetActiveCommandBuffer(), {});
		m_main_render_texture.Clear(renderer.GetActiveCommandBuffer(), Vec4f{ 0.0f, 0.0f, 0.0f, 1.0f });
		m_main_render_texture.TransitionLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, renderer.GetActiveCommandBuffer());

		if(scene.GetDescription().render_3D_enabled)
			m_forward.Pass(scene, renderer, m_main_render_texture);
		if(scene.GetDescription().render_skybox_enabled)
			m_skybox.Pass(scene, renderer, m_main_render_texture);
		if(scene.GetDescription().render_2D_enabled)
			m_2Dpass.Pass(scene, renderer, m_main_render_texture);

		m_final.Pass(scene, renderer, m_main_render_texture);
	}

	void RenderPasses::Destroy()
	{
		RenderCore::Get().WaitDeviceIdle();
		m_skybox.Destroy();
		m_2Dpass.Destroy();
		m_final.Destroy();
		m_main_render_texture.Destroy();
	}
}
