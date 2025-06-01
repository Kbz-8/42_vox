#include <Renderer/RenderPasses/PostProcessPass.h>
#include <Renderer/Pipelines/Graphics.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>
#include <Core/EventBus.h>
#include <Core/Engine.h>

namespace Scop
{
	void PostProcessPass::Init()
	{
		ShaderLayout vertex_shader_layout(
			{}, {}
		);
		p_vertex_shader = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/ScreenVertex.spv", ShaderType::Vertex, std::move(vertex_shader_layout));

		std::function<void(const EventBase&)> functor = [this](const EventBase& event)
		{
			if(event.What() == Event::ResizeEventCode)
			{
				m_render_texture.Destroy();
				m_pipeline.Destroy();
			}
		};
		EventBus::RegisterListener({ functor, "__ScopPostProcessPass" });
	}

	void PostProcessPass::Pass(Scene& scene, Renderer& renderer, Texture& render_target)
	{
		Scene::PostProcessData& data = scene.GetPostProcessData();

		if(!m_render_texture.IsInit())
		{
			auto extent = kvfGetSwapchainImagesSize(renderer.GetSwapchain().Get());
			m_render_texture.Init({}, extent.width, extent.height, VK_FORMAT_R8G8B8A8_UNORM, false, "scop_post_process_render_texture", false);
		}

		if(m_pipeline.GetPipeline() == VK_NULL_HANDLE)
		{
			GraphicPipelineDescriptor pipeline_descriptor;
			pipeline_descriptor.vertex_shader = p_vertex_shader;
			pipeline_descriptor.fragment_shader = scene.GetDescription().post_process_shader;
			pipeline_descriptor.color_attachments = { &m_render_texture };
			pipeline_descriptor.culling = CullMode::None;
			pipeline_descriptor.clear_color_attachments = false;
			pipeline_descriptor.no_vertex_inputs = true;
			pipeline_descriptor.name = "post_process_pass_pipeline";
			m_pipeline.Init(std::move(pipeline_descriptor));
		}

		VkCommandBuffer cmd = renderer.GetActiveCommandBuffer();

		data.set->SetImage(renderer.GetCurrentFrameIndex(), 0, render_target);
		data.set->SetImage(renderer.GetCurrentFrameIndex(), 1, scene.GetDepth());
		data.set->SetUniformBuffer(renderer.GetCurrentFrameIndex(), 2, data.data_buffer->Get(renderer.GetCurrentFrameIndex()));
		data.set->Update(renderer.GetCurrentFrameIndex(), cmd);

		m_pipeline.BindPipeline(cmd, 0, {});
			VkDescriptorSet set = data.set->GetSet(renderer.GetCurrentFrameIndex());
			RenderCore::Get().vkCmdBindDescriptorSets(cmd, m_pipeline.GetPipelineBindPoint(), m_pipeline.GetPipelineLayout(), 0, 1, &set, 0, nullptr);
			RenderCore::Get().vkCmdDraw(cmd, 3, 1, 0, 0);
			renderer.GetDrawCallsCounterRef()++;
			renderer.GetPolygonDrawnCounterRef()++;
		m_pipeline.EndPipeline(cmd);
	}

	void PostProcessPass::Destroy()
	{
		RenderCore::Get().WaitDeviceIdle();
		m_render_texture.Destroy();
		m_pipeline.Destroy();
		p_vertex_shader.reset();
	}
}
