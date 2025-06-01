#include <Renderer/RenderPasses/ForwardPass.h>
#include <Renderer/Pipelines/Graphics.h>
#include <Renderer/ViewerData.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>
#include <Maths/Mat4.h>

#include <map>

namespace Scop
{
	struct ModelData
	{
		Mat4f model_mat;
		Mat4f normal_mat;
	};

	void ForwardPass::Pass(Scene& scene, Renderer& renderer, class Texture& render_target)
	{
		NonOwningPtr<GraphicPipeline> pipeline = &scene.GetPipeline();
		if(scene.GetPipeline().GetPipeline() == VK_NULL_HANDLE)
		{
			GraphicPipelineDescriptor pipeline_descriptor;
			pipeline_descriptor.vertex_shader = RenderCore::Get().GetDefaultVertexShader();
			pipeline_descriptor.fragment_shader = scene.GetFragmentShader();
			pipeline_descriptor.color_attachments = { &render_target };
			pipeline_descriptor.depth = &scene.GetDepth();
			pipeline_descriptor.clear_color_attachments = false;
			pipeline_descriptor.name = "forward_pass_pipeline";
			scene.GetPipeline().Init(std::move(pipeline_descriptor));
		}

		auto render_actor = [&render_target, &renderer, &scene, &pipeline](Actor& actor)
		{
			Scene::ForwardData& data = scene.GetForwardData();
			VkCommandBuffer cmd = renderer.GetActiveCommandBuffer();
			std::shared_ptr<GraphicPipeline> custom_pipeline = (actor.GetCustomPipeline().has_value() ? actor.GetCustomPipeline()->pipeline : nullptr);

			if(custom_pipeline && !custom_pipeline->IsPipelineBound())
			{
				pipeline->EndPipeline(cmd);
				pipeline = actor.GetCustomPipeline()->pipeline.get();
				if(pipeline->GetDescription().depth != NonOwningPtr<DepthImage>{ &scene.GetDepth() })
				{
					GraphicPipelineDescriptor descriptor = pipeline->GetDescription();
					pipeline->Destroy();
					descriptor.color_attachments = { &render_target };
					descriptor.depth = &scene.GetDepth();
					descriptor.renderer = nullptr;
					descriptor.clear_color_attachments = false;
					pipeline->Init(std::move(descriptor));
				}
				pipeline->BindPipeline(cmd, 0, {});
			}
			else if(!custom_pipeline && !scene.GetPipeline().IsPipelineBound())
			{
				pipeline->EndPipeline(cmd);
				pipeline = &scene.GetPipeline();
				pipeline->BindPipeline(cmd, 0, {});
			}

			ModelData model_data;
			model_data.model_mat = Mat4f::Identity();
			model_data.model_mat.SetTranslation(actor.GetPosition() - actor.GetModel().GetCenter());
			model_data.model_mat.SetScale(actor.GetScale());
			model_data.model_mat = Mat4f::Translate(-actor.GetModel().GetCenter()) * Mat4f::Rotate(actor.GetOrientation()) * model_data.model_mat;
			model_data.normal_mat = model_data.model_mat;
			model_data.normal_mat.Inverse().Transpose();

			RenderCore::Get().vkCmdPushConstants(cmd, pipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ModelData), &model_data);

			if(custom_pipeline)
			{
				if(!actor.GetCustomPipeline()->data_uniform_buffer)
				{
					actor.GetCustomPipeline()->data_uniform_buffer = std::make_shared<UniformBuffer>();
					actor.GetCustomPipeline()->data_uniform_buffer->Init(actor.GetCustomPipeline()->data.GetSize(), "custom_data_buffer");
				}
				else
					actor.GetCustomPipeline()->data_uniform_buffer->SetData(actor.GetCustomPipeline()->data, renderer.GetCurrentFrameIndex());
				if(!actor.GetCustomPipeline()->set)
				{
					actor.GetCustomPipeline()->set = RenderCore::Get().GetDescriptorPoolManager().GetAvailablePool().RequestDescriptorSet(custom_pipeline->GetDescription().vertex_shader->GetShaderLayout().set_layouts.at(0), ShaderType::Vertex);
					for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
					{
						actor.GetCustomPipeline()->set->SetUniformBuffer(i, 0, data.matrices_buffer->Get(i));
						actor.GetCustomPipeline()->set->SetUniformBuffer(i, 1, actor.GetCustomPipeline()->data_uniform_buffer->Get(i));
						actor.GetCustomPipeline()->set->Update(i);
					}
				}
				actor.GetModel().Draw(cmd, actor.GetCustomPipeline()->set, *pipeline, data.albedo_set, renderer.GetDrawCallsCounterRef(), renderer.GetPolygonDrawnCounterRef(), renderer.GetCurrentFrameIndex());
			}
			else
				actor.GetModel().Draw(cmd, data.matrices_set, *pipeline, data.albedo_set, renderer.GetDrawCallsCounterRef(), renderer.GetPolygonDrawnCounterRef(), renderer.GetCurrentFrameIndex());
		};

		std::multimap<float, Actor&> sorted_actors;
		for(auto& [_, actor] : scene.GetActors())
		{
			if(!actor.IsVisible())
				continue;
			if(!actor.IsOpaque())
			{
				float distance = Vec3f::Distance(actor.GetPosition(), scene.GetCamera()->GetPosition());
				sorted_actors.emplace(distance, const_cast<Actor&>(actor));
			}
			else
				render_actor(const_cast<Actor&>(actor));
		}
		for(auto it = sorted_actors.rbegin(); it != sorted_actors.rend(); ++it)
		{
			if(!it->second.IsVisible())
				continue;
			render_actor(it->second);
		}
		if(pipeline)
			pipeline->EndPipeline(renderer.GetActiveCommandBuffer());
	}
}
