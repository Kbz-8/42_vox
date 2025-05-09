#ifndef BLOCK_H
#define BLOCK_H

#include <vector>

#include <ScopGraphics.h>

static const std::vector<Scop::Vertex> BLOCK_MESH = {
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f,  1.0f, 1.0f }, Scop::Vec2f{ 0, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f,  1.0f, 1.0f }, Scop::Vec2f{ 1, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f,  1.0f, 1.0f }, Scop::Vec2f{ 1, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f,  1.0f, 1.0f }, Scop::Vec2f{ 1, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f,  1.0f, 1.0f }, Scop::Vec2f{ 0, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f,  1.0f, 1.0f }, Scop::Vec2f{ 0, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f, -1.0f, 1.0f }, Scop::Vec2f{ 0, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f, -1.0f, 1.0f }, Scop::Vec2f{ 1, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f, -1.0f, 1.0f }, Scop::Vec2f{ 1, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f, -1.0f, 1.0f }, Scop::Vec2f{ 1, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f, -1.0f, 1.0f }, Scop::Vec2f{ 0, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f, -1.0f, 1.0f }, Scop::Vec2f{ 0, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f, -1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f, -1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f, -1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f, -1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f, -1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f, -1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{ -1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{ -1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{ -1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{ -1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{ -1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{ -1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1, 1 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0, 0 } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0, 1 } }
};

#endif
