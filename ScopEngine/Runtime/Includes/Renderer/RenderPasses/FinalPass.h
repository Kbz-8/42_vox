#ifndef __SCOP_FINAL_PASS__
#define __SCOP_FINAL_PASS__

#include <memory>

#include <Renderer/Descriptor.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/Pipelines/Graphics.h>

namespace Scop
{
	class FinalPass
	{
		public:
			FinalPass() = default;
			void Init();
			void Pass(class Scene& scene, class Renderer& renderer, class Texture& render_target);
			void Destroy();
			~FinalPass() = default;

		private:
			GraphicPipeline m_pipeline;
			std::shared_ptr<DescriptorSet> p_set;
			std::shared_ptr<Shader> p_vertex_shader;
			std::shared_ptr<Shader> p_fragment_shader;
	};
}

#endif
