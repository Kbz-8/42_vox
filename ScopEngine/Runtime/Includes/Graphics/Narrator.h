#ifndef __SCOP_GRAPHICS_NARRATOR__
#define __SCOP_GRAPHICS_NARRATOR__

#include <Maths/Vec3.h>
#include <Maths/Vec4.h>
#include <Maths/Quaternions.h>
#include <Core/Script.h>
#include <Graphics/Model.h>

namespace Scop
{
	class Narrator
	{
		friend Scene;

		public:
			Narrator() = default;
			inline void AttachScript(std::shared_ptr<NarratorScript> script) { p_script = script; }
			inline ~Narrator()
			{
				if(p_script)
					p_script->OnQuit();
			}

		private:
			inline void Update(NonOwningPtr<class Scene> scene, class Inputs& input, float timestep)
			{
				if(p_script)
					p_script->OnUpdate(scene, input, timestep);
			}

		private:
			std::shared_ptr<NarratorScript> p_script;
	};
}

#endif

