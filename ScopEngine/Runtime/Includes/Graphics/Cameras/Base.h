#ifndef __SCOP_CAMERAS_BASE__
#define __SCOP_CAMERAS_BASE__

#include <Maths/Mat4.h>

namespace Scop
{
	class BaseCamera
	{
		public:
			BaseCamera() = default;

			virtual void Update(class Inputs& input, float aspect, float timestep) {};

			[[nodiscard]] inline const Mat4f& GetView() const noexcept { return m_view; }
			[[nodiscard]] inline const Mat4f& GetProj() const noexcept { return m_proj; }

			[[nodiscard]] virtual const Vec3f& GetPosition() const noexcept = 0;

			virtual constexpr std::string GetCameraType() = 0;

			virtual ~BaseCamera() = default;

		protected:
			Mat4f m_view;
			Mat4f m_proj;
	};
}

#endif
