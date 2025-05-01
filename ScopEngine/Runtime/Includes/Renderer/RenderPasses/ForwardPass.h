#ifndef __SCOP_FORWARD_PASS__
#define __SCOP_FORWARD_PASS__

namespace Scop
{
	class ForwardPass
	{
		public:
			ForwardPass() = default;
			void Pass(class Scene& scene, class Renderer& renderer, class Texture& render_target);
			~ForwardPass() = default;
	};
}

#endif
