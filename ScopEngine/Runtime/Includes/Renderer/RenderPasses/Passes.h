#ifndef __SCOP_PASSES__
#define __SCOP_PASSES__

#include <Renderer/Image.h>
#include <Renderer/RenderPasses/SkyboxPass.h>
#include <Renderer/RenderPasses/ForwardPass.h>
#include <Renderer/RenderPasses/FinalPass.h>
#include <Renderer/RenderPasses/PostProcessPass.h>
#include <Renderer/RenderPasses/2DPass.h>

namespace Scop
{
	class RenderPasses
	{
		public:
			RenderPasses() = default;
			void Init();
			void Pass(class Scene& scene, class Renderer& renderer);
			void Destroy();
			~RenderPasses() = default;

		private:
			SkyboxPass m_skybox;
			Render2DPass m_2Dpass;
			PostProcessPass m_post_process;
			FinalPass m_final;
			Texture m_main_render_texture;
			ForwardPass m_forward;
	};
}

#endif
