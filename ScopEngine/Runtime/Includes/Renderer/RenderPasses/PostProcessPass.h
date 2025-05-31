#ifndef __SCOP_POST_PROCESS_PASS__
#define __SCOP_POST_PROCESS_PASS__

#include <Renderer/Descriptor.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/Pipelines/Graphics.h>

namespace Scop
{
	class PostProcessPass
	{
		public:
			PostProcessPass() = default;
			void Init();
			void Pass(class Scene& scene, class Renderer& renderer, class Texture& render_target);
			void Destroy();

			[[nodiscard]] inline Texture& GetProcessTexture() noexcept { return m_render_texture; }
			~PostProcessPass() = default;

		private:
			GraphicPipeline m_pipeline;
			Texture m_render_texture;
			std::shared_ptr<Shader> p_vertex_shader;
	};
}

#endif
