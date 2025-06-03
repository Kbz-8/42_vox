#ifndef __SCOP_CAMERAS_FIRST_PERSON_3D__
#define __SCOP_CAMERAS_FIRST_PERSON_3D__

#include <Graphics/Cameras/Base.h>
#include <Maths/Vec3.h>

namespace Scop
{
	class FirstPerson3D : public BaseCamera
	{
		public:
			FirstPerson3D();
			FirstPerson3D(Vec3f position, float fov = 90.0f, float speed = 50.0f);

			void Update(class Inputs& input, float aspect, float timestep) override;

			inline constexpr void EnableCamera() noexcept { m_inputs_blocked = false; }
			inline constexpr void DisableCamera() noexcept { m_inputs_blocked = true; }

			[[nodiscard]] inline constexpr std::string GetCameraType() override { return "FirstPerson3D"; }
			[[nodiscard]] const Vec3f& GetPosition() const noexcept override { return m_position; }
			[[nodiscard]] const Vec3f& GetUp() const noexcept { return c_up; }
			[[nodiscard]] const Vec3f& GetLeft() const noexcept { return m_left; }
			[[nodiscard]] const Vec3f& GetTarget() const noexcept { return m_target; }
			[[nodiscard]] const Vec3f& GetDirection() const noexcept { return m_direction; }

			~FirstPerson3D() = default;

		private:
			void UpdateView();

		private:
			const Vec3f c_up;
			Vec3f m_position;
			Vec3f m_left;
			Vec3f m_forward;
			Vec3f m_target;
			Vec3f m_direction;
			Vec3f m_mov;

			float m_theta = 0.0;
			float m_phi = 0.0;

			const float c_speed = 50.0f;
			const float c_sensivity = 0.7f;
			float m_speed_factor = 1.0f;
			float m_fov = 90.0f;

			bool m_inputs_blocked = false;
	};
}

#endif
