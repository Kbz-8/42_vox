#ifndef __SCOP_MESH_FACTORY__
#define __SCOP_MESH_FACTORY__

#include <memory>

#include <Maths/Vec2.h>
#include <Maths/Vec3.h>

namespace Scop
{
	std::shared_ptr<class Mesh> CreateQuad();
	std::shared_ptr<class Mesh> CreateQuad(float x, float y, float width, float height);
	std::shared_ptr<class Mesh> CreateQuad(const Vec2f& position, const Vec2f& size);
	std::shared_ptr<class Mesh> CreateCube();
	std::shared_ptr<class Mesh> CreatePyramid();
	std::shared_ptr<class Mesh> CreateSphere(std::uint32_t x_segments = 32, std::uint32_t y_segments = 32);
	std::shared_ptr<class Mesh> CreateCapsule(float radius = 0.5f, float mid_height = 2.0f, int radial_degments = 64, int rings = 8);
	std::shared_ptr<class Mesh> CreatePlane(float width, float height, const Vec3f& normal);
}

#endif
