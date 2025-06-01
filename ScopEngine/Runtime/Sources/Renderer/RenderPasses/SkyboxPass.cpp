#include <Renderer/RenderPasses/SkyboxPass.h>
#include <Renderer/Pipelines/Graphics.h>
#include <Graphics/MeshFactory.h>
#include <Renderer/ViewerData.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>
#include <Core/EventBus.h>
#include <Core/Engine.h>

namespace Scop
{
	void SkyboxPass::Init()
	{
		ShaderLayout vertex_shader_layout(
			{
				{ 0,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }
					})
				}
			}, {}
		);
		p_vertex_shader = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/SkyboxVertex.spv", ShaderType::Vertex, std::move(vertex_shader_layout));
		ShaderLayout fragment_shader_layout(
			{
				{ 1,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }
					})
				}
			}, {}
		);
		p_fragment_shader = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/SkyboxFragment.spv", ShaderType::Fragment, std::move(fragment_shader_layout));

		std::function<void(const EventBase&)> functor = [this](const EventBase& event)
		{
			if(event.What() == Event::ResizeEventCode || event.What() == Event::SceneHasChangedEventCode)
				m_pipeline.Destroy();
		};
		EventBus::RegisterListener({ functor, "__ScopSkyboxPass" });

		m_cube = CreateCube();
		p_set = RenderCore::Get().GetDescriptorPoolManager().GetAvailablePool().RequestDescriptorSet(p_fragment_shader->GetShaderLayout().set_layouts.at(1), ShaderType::Fragment);
	}

	void SkyboxPass::Pass(Scene& scene, Renderer& renderer, class Texture& render_target)
	{
		if(!scene.GetSkybox())
			return;

		if(m_pipeline.GetPipeline() == VK_NULL_HANDLE)
		{
			GraphicPipelineDescriptor pipeline_descriptor;
			pipeline_descriptor.vertex_shader = p_vertex_shader;
			pipeline_descriptor.fragment_shader = p_fragment_shader;
			pipeline_descriptor.color_attachments = { &render_target };
			pipeline_descriptor.depth = &scene.GetDepth();
			pipeline_descriptor.culling = CullMode::None;
			pipeline_descriptor.depth_test_equal = true;
			pipeline_descriptor.clear_color_attachments = false;
			pipeline_descriptor.name = "skybox_pass_pipeline";
			m_pipeline.Init(std::move(pipeline_descriptor));
		}

		VkCommandBuffer cmd = renderer.GetActiveCommandBuffer();

		p_set->SetImage(renderer.GetCurrentFrameIndex(), 0, *scene.GetSkybox());
		p_set->Update(renderer.GetCurrentFrameIndex(), cmd);

		m_pipeline.BindPipeline(cmd, 0, {});
			std::array<VkDescriptorSet, 2> sets = { scene.GetForwardData().matrices_set->GetSet(renderer.GetCurrentFrameIndex()), p_set->GetSet(renderer.GetCurrentFrameIndex()) };
			RenderCore::Get().vkCmdBindDescriptorSets(cmd, m_pipeline.GetPipelineBindPoint(), m_pipeline.GetPipelineLayout(), 0, sets.size(), sets.data(), 0, nullptr);
			m_cube->Draw(cmd, renderer.GetDrawCallsCounterRef(), renderer.GetPolygonDrawnCounterRef());
		m_pipeline.EndPipeline(cmd);
	}

	void SkyboxPass::Destroy()
	{
		RenderCore::Get().WaitDeviceIdle();
		m_pipeline.Destroy();
		p_vertex_shader.reset();
		p_fragment_shader.reset();
		m_cube.reset();
		p_set.reset();
	}
}
