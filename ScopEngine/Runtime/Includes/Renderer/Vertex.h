#ifndef __SCOP_VERTEX__
#define __SCOP_VERTEX__

#include <kvf.h>
#include <array>
#include <Maths/Vec4.h>
#include <Maths/Vec2.h>

namespace Scop
{
	struct Vertex
	{
		alignas(16) Vec4f position = Vec4f{ 0.0f, 0.0f, 0.0f, 1.0f };
		alignas(16) Vec4f color = Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f };
		alignas(16) Vec4f normal = Vec4f{ 0.0f, 0.0f, 0.0f, 1.0f };
		alignas(16) Vec2f uv = Vec2f{ 0.0f, 0.0f };

		Vertex() = default;
		Vertex(Vec4f p, Vec4f c, Vec4f n, Vec2f u) : position(std::move(p)), color(std::move(c)), normal(std::move(n)), uv(std::move(u)) {}

		[[nodiscard]] inline static VkVertexInputBindingDescription GetBindingDescription();
		[[nodiscard]] inline static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions();
	};
}

#include <Renderer/Vertex.inl>

#endif
