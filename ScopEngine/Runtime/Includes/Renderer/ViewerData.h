#ifndef __SCOP_VIEWER_DATA__
#define __SCOP_VIEWER_DATA__

#include <Maths/Mat4.h>
#include <Maths/Vec3.h>

namespace Scop
{
	struct ViewerData
	{
		Mat4f projection_matrix;
		Mat4f inv_projection_matrix;
		Mat4f view_matrix;
		Mat4f inv_view_matrix;
		Mat4f view_proj_matrix;
		Mat4f inv_view_proj_matrix;
		alignas(16) Vec3f camera_position;
	};
}

#endif
