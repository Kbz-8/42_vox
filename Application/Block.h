#ifndef BLOCK_H
#define BLOCK_H

#include <vector>

#include <ScopGraphics.h>

static const std::vector<Scop::Vertex> BLOCK_MESH = {
	//Front face
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f,  1.0f, 1.0f }, Scop::Vec2f{ 0.0f, 0.0f } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f,  1.0f, 1.0f }, Scop::Vec2f{ 0.0f, 1.0f } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f,  1.0f, 1.0f }, Scop::Vec2f{ 1.0f, 0.0f } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f,  1.0f, 1.0f }, Scop::Vec2f{ 1.0f, 1.0f } },
	//Back face
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f, -1.0f, 1.0f }, Scop::Vec2f{ 0.0f, 0.0f } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f, -1.0f, 1.0f }, Scop::Vec2f{ 0.0f, 1.0f } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f, -1.0f, 1.0f }, Scop::Vec2f{ 1.0f, 0.0f } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  0.0f, -1.0f, 1.0f }, Scop::Vec2f{ 1.0f, 1.0f } },
	//Top face
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0.0f, 0.0f } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0.0f, 1.0f } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f,  1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1.0f, 0.0f } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f,  1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1.0f, 1.0f } },
	//Bottom face
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f, -1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0.0f, 0.0f } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f, -1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0.0f, 1.0f } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{  0.0f, -1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1.0f, 0.0f } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{  0.0f, -1.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1.0f, 1.0f } },
	//Right face
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{  1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0.0f, 0.0f } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{  1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0.0f, 1.0f } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{  1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1.0f, 0.0f } },
	Scop::Vertex{ Scop::Vec4f{ 1.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{  1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1.0f, 1.0f } },
	//Left face
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 0.0f, 1.0f }, Scop::Vec4f{ -1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0.0f, 0.0f } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 0.0f, 1.0f }, Scop::Vec4f{ -1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 0.0f, 1.0f } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 0.0f, 1.0f, 1.0f }, Scop::Vec4f{ -1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1.0f, 0.0f } },
	Scop::Vertex{ Scop::Vec4f{ 0.0f, 1.0f, 1.0f, 1.0f }, Scop::Vec4f{ -1.0f,  0.0f,  0.0f, 1.0f }, Scop::Vec2f{ 1.0f, 1.0f } }
};

#endif
