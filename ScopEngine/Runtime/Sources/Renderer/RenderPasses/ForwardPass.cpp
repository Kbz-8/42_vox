#include <Renderer/RenderPasses/ForwardPass.h>
#include <Renderer/Pipelines/Graphics.h>
#include <Renderer/ViewerData.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>
#include <Maths/Mat4.h>

namespace Scop
{
	struct ModelData
	{
		Mat4f model_mat;
		Mat4f normal_mat;
	};

	void ForwardPass::Pass(Scene& scene, Renderer& renderer, class Texture& render_target)
	{
		Scene::ForwardData& data = scene.GetForwardData();
		GraphicPipeline& pipeline = scene.GetPipeline();

		if(pipeline.GetPipeline() == VK_NULL_HANDLE)
		{
			GraphicPipelineDescriptor pipeline_descriptor;
			pipeline_descriptor.vertex_shader = RenderCore::Get().GetDefaultVertexShader();
			pipeline_descriptor.fragment_shader = scene.GetFragmentShader();
			pipeline_descriptor.color_attachments = { &render_target };
			pipeline_descriptor.depth = &scene.GetDepth();
			if(scene.GetForwardData().wireframe)
				pipeline_descriptor.mode = VK_POLYGON_MODE_LINE;
			pipeline_descriptor.clear_color_attachments = false;
			pipeline_descriptor.name = "forward_pass_pipeline";
			switch(scene.GetDescription().culling)
			{
				case CullMode::None: pipeline_descriptor.culling = VK_CULL_MODE_NONE; break;
				case CullMode::Back: pipeline_descriptor.culling = VK_CULL_MODE_BACK_BIT; break;
				case CullMode::Front: pipeline_descriptor.culling = VK_CULL_MODE_FRONT_BIT; break;
				case CullMode::FrontAndBack: pipeline_descriptor.culling = VK_CULL_MODE_FRONT_AND_BACK; break;

				default: break;
			}
			pipeline.Init(pipeline_descriptor);
		}

		VkCommandBuffer cmd = renderer.GetActiveCommandBuffer();
		pipeline.BindPipeline(cmd, 0, {});
		for(auto actor : scene.GetActors())
		{
			if(!actor->IsVisible())
				continue;
			ModelData model_data;
			model_data.model_mat = Mat4f::Identity();
			model_data.model_mat.SetTranslation(actor->GetPosition() - actor->GetModel().GetCenter());
			model_data.model_mat.SetScale(actor->GetScale());
			model_data.model_mat = Mat4f::Translate(-actor->GetModel().GetCenter()) * Mat4f::Rotate(actor->GetOrientation()) * model_data.model_mat;
			model_data.normal_mat = model_data.model_mat;
			model_data.normal_mat.Inverse().Transpose();
			RenderCore::Get().vkCmdPushConstants(cmd, pipeline.GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ModelData), &model_data);
			actor->GetModel().Draw(cmd, *data.matrices_set, pipeline, *data.albedo_set, renderer.GetDrawCallsCounterRef(), renderer.GetPolygonDrawnCounterRef(), renderer.GetCurrentFrameIndex());
		}
		pipeline.EndPipeline(cmd);
	}
}
