#ifndef __SCOP_GRAPHICS_NARRATOR__
#define __SCOP_GRAPHICS_NARRATOR__

#include <Maths/Vec3.h>
#include <Maths/Vec4.h>
#include <Maths/Quaternions.h>
#include <Core/Script.h>
#include <Graphics/Model.h>
#include <Core/UUID.h>

namespace Scop
{
	class Narrator
	{
		friend Scene;

		public:
			Narrator() : m_uuid(UUID()) {}
			Narrator(std::uint64_t uuid) : m_uuid(uuid) {}
			inline void AttachScript(std::shared_ptr<NarratorScript> script) { p_script = script; }
			[[nodiscard]] inline std::uint64_t GetUUID() const noexcept { return m_uuid; }
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
			std::uint64_t m_uuid;
	};
}

namespace std
{
	template <>
	struct hash<Scop::Narrator>
	{
		std::size_t operator()(const Scop::Narrator& n) const noexcept
		{
			return static_cast<std::size_t>(n.GetUUID());
		}
	};
}

#endif
