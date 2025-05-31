#include <Renderer/ScenesRenderer.h>
#include <Renderer/Renderer.h>
#include <Graphics/Scene.h>
#include <Renderer/ViewerData.h>

#include <cstring>

namespace Scop
{
	void SceneRenderer::Init()
	{
		m_passes.Init();
	}

	void SceneRenderer::Render(Scene& scene, Renderer& renderer)
	{
		if(scene.GetCamera())
		{
			ViewerData data;
			data.projection_matrix = scene.GetCamera()->GetProj();
			data.projection_matrix.GetInverse(&data.inv_projection_matrix);
			data.view_matrix = scene.GetCamera()->GetView();
			data.view_matrix.GetInverse(&data.inv_view_matrix);
			data.view_proj_matrix = data.view_matrix * data.projection_matrix;
			data.view_proj_matrix.GetInverse(&data.inv_view_proj_matrix);
			data.camera_position = scene.GetCamera()->GetPosition();

			static CPUBuffer buffer(sizeof(ViewerData));
			std::memcpy(buffer.GetData(), &data, buffer.GetSize());
			scene.GetForwardData().matrices_buffer->SetData(buffer, renderer.GetCurrentFrameIndex());
		}
		if(scene.GetDescription().render_post_process_enabled && scene.GetDescription().post_process_shader)
			scene.GetPostProcessData().data_buffer->SetData(scene.GetPostProcessData().data, renderer.GetCurrentFrameIndex());

		m_passes.Pass(scene, renderer);
	}

	void SceneRenderer::Destroy()
	{
		RenderCore::Get().WaitDeviceIdle();
		m_passes.Destroy();
	}
}
