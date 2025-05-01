#ifndef __SCOP_2D_PASS__
#define __SCOP_2D_PASS__

#include <memory>

#include <Renderer/Descriptor.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/Pipelines/Graphics.h>

namespace Scop
{
	class Render2DPass
	{
		public:
			Render2DPass() = default;
			void Init();
			void Pass(class Scene& scene, class Renderer& renderer, class Texture& render_target);
			void Destroy();
			~Render2DPass() = default;

		private:
			GraphicPipeline m_pipeline;
			std::shared_ptr<DescriptorSet> p_viewer_data_set;
			std::shared_ptr<UniformBuffer> p_viewer_data_buffer;
			std::shared_ptr<DescriptorSet> p_texture_set;
			std::shared_ptr<Shader> p_vertex_shader;
			std::shared_ptr<Shader> p_fragment_shader;
	};
}

#endif
