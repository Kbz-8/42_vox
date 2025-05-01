#ifndef __SCOP_SCENES_RENDERER__
#define __SCOP_SCENES_RENDERER__

#include <Renderer/RenderPasses/Passes.h>

namespace Scop
{
	class SceneRenderer
	{
		public:
			SceneRenderer() = default;
			void Init();
			void Render(class Scene& scene, class Renderer& renderer); // TODO : add RTT support
			void Destroy();
			~SceneRenderer() = default;

		private:
			RenderPasses m_passes;
	};
}

#endif
