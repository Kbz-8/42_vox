#ifndef __SCOP_GRAPHICS_ACTOR__
#define __SCOP_GRAPHICS_ACTOR__

#include <Maths/Vec3.h>
#include <Maths/Vec4.h>
#include <Maths/Quaternions.h>
#include <Core/Script.h>
#include <Graphics/Model.h>
#include <Core/UUID.h>

namespace Scop
{
	class Actor
	{
		friend Scene;

		public:
			Actor();
			Actor(Model model);
			Actor(std::uint64_t uuid, Model model);

			inline void AttachScript(std::shared_ptr<ActorScript> script) { p_script = script; }

			inline void SetColor(Vec4f color) noexcept { m_color = color; }
			inline void SetPosition(Vec3f position) noexcept { m_position = position; }
			inline void SetScale(Vec3f scale) noexcept { m_scale = scale; }
			inline void SetOrientation(Quatf orientation) noexcept { m_orientation = orientation; }
			inline void SetVisibility(bool show) noexcept { m_is_visible = show; }

			[[nodiscard]] inline const Vec4f& GetColor() const noexcept { return m_color; }
			[[nodiscard]] inline const Vec3f& GetPosition() const noexcept { return m_position; }
			[[nodiscard]] inline const Vec3f& GetScale() const noexcept { return m_scale; }
			[[nodiscard]] inline const Quatf& GetOrientation() const noexcept { return m_orientation; }
			[[nodiscard]] inline const Model& GetModel() const noexcept { return m_model; }
			[[nodiscard]] inline Model& GetModelRef() noexcept { return m_model; }
			[[nodiscard]] inline std::uint64_t GetUUID() const noexcept { return m_uuid; }
			[[nodiscard]] inline bool IsVisible() const noexcept { return m_is_visible; }

			~Actor();

		public:
			void Update(NonOwningPtr<class Scene> scene, class Inputs& input, float timestep);

		private:
			Model m_model;
			Quatf m_orientation = Quatf::Identity();
			Vec4f m_color = Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f };
			Vec3f m_position = Vec3f{ 0.0f, 0.0f, 0.0f };
			Vec3f m_scale = Vec3f{ 1.0f, 1.0f, 1.0f };
			std::shared_ptr<ActorScript> p_script;
			std::uint64_t m_uuid;
			bool m_is_visible = true;
	};
}

namespace std
{
	template <>
	struct hash<Scop::Actor>
	{
		std::size_t operator()(const Scop::Actor& a) const noexcept
		{
			return static_cast<std::size_t>(a.GetUUID());
		}
	};
}

#endif
