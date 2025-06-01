#ifndef __SCOP_GRAPHICS_ACTOR__
#define __SCOP_GRAPHICS_ACTOR__

#include <optional>

#include <Core/UUID.h>
#include <Maths/Vec3.h>
#include <Maths/Vec4.h>
#include <Core/Script.h>
#include <Maths/Quaternions.h>
#include <Graphics/Model.h>

namespace Scop
{
	class Actor
	{
		friend Scene;

		public:
			struct CustomPipeline
			{
				std::shared_ptr<GraphicPipeline> pipeline;
				std::shared_ptr<DescriptorSet> set;
				std::shared_ptr<UniformBuffer> data_uniform_buffer;
				CPUBuffer data;
			};

		public:
			Actor();
			Actor(Model model);
			Actor(std::uint64_t uuid, Model model);

			inline void AttachScript(std::shared_ptr<ActorScript> script) { p_script = script; }

			inline void SetPosition(Vec3f position) noexcept { m_position = position; }
			inline void SetScale(Vec3f scale) noexcept { m_scale = scale; }
			inline void SetOrientation(Quatf orientation) noexcept { m_orientation = orientation; }
			inline void SetVisibility(bool show) noexcept { m_is_visible = show; }
			inline void SetIsOpaque(bool opaque) noexcept { m_is_opaque = opaque; }
			inline void SetCustomPipeline(const CustomPipeline& pipeline) { m_custom_pipeline = pipeline; }

			[[nodiscard]] inline const Vec3f& GetPosition() const noexcept { return m_position; }
			[[nodiscard]] inline const Vec3f& GetScale() const noexcept { return m_scale; }
			[[nodiscard]] inline const Quatf& GetOrientation() const noexcept { return m_orientation; }
			[[nodiscard]] inline const Model& GetModel() const noexcept { return m_model; }
			[[nodiscard]] inline Model& GetModelRef() noexcept { return m_model; }
			[[nodiscard]] inline std::uint64_t GetUUID() const noexcept { return m_uuid; }
			[[nodiscard]] inline bool IsVisible() const noexcept { return m_is_visible; }
			[[nodiscard]] inline bool IsOpaque() const noexcept { return m_is_opaque; }
			[[nodiscard]] inline std::optional<CustomPipeline>& GetCustomPipeline() { return m_custom_pipeline; }

			~Actor();

		public:
			void Update(NonOwningPtr<class Scene> scene, class Inputs& input, float timestep);

		private:
			Model m_model;
			Quatf m_orientation = Quatf::Identity();
			Vec3f m_position = Vec3f{ 0.0f, 0.0f, 0.0f };
			Vec3f m_scale = Vec3f{ 1.0f, 1.0f, 1.0f };
			std::shared_ptr<ActorScript> p_script;
			std::uint64_t m_uuid;
			std::optional<CustomPipeline> m_custom_pipeline;
			bool m_is_visible = true;
			bool m_is_opaque = true;
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
