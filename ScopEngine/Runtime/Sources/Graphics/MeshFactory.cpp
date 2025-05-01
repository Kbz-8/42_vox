#include <Graphics/MeshFactory.h>
#include <Graphics/Mesh.h>
#include <Renderer/Vertex.h>
#include <Maths/Constants.h>
#include <Maths/Quaternions.h>

#include <cmath>
#include <vector>

namespace Scop
{
	std::shared_ptr<Mesh> CreateQuad(float x, float y, float width, float height)
	{
		std::vector<Vertex> data(4);

		data[0].position = Vec4f(x, y, 0.0f, 1.0f);
		data[0].uv = Vec2f(1.0f, 1.0f);

		data[1].position = Vec4f(x + width, y, 0.0f, 1.0f);
		data[1].uv = Vec2f(0.0f, 1.0f);

		data[2].position = Vec4f(x + width, y + height, 0.0f, 1.0f);
		data[2].uv = Vec2f(0.0f, 0.0f);

		data[3].position = Vec4f(x, y + height, 0.0f, 1.0f);
		data[3].uv = Vec2f(1.0f, 0.0f);

		std::vector<std::uint32_t> indices = {
			0,
			1,
			2,
			2,
			3,
			0,
		};

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->AddSubMesh({ std::move(data), std::move(indices) });
		return mesh;
	}

	std::shared_ptr<Mesh> CreateQuad(const Vec2f& position, const Vec2f& size)
	{
		return CreateQuad(position.x, position.y, size.x, size.y);
	}

	std::shared_ptr<Mesh> CreateQuad()
	{
		std::vector<Vertex> data(4);

		data[0].position = Vec4f(-1.0f, -1.0f, 0.0f, 1.0f);
		data[0].uv = Vec2f(0.0f, 0.0f);
		data[0].color = Vec4f(1.0f);

		data[1].position = Vec4f(1.0f, -1.0f, 0.0f, 1.0f);
		data[1].color = Vec4f(1.0f);
		data[1].uv = Vec2f(1.0f, 0.0f);

		data[2].position = Vec4f(1.0f, 1.0f, 0.0f, 1.0f);
		data[2].color = Vec4f(1.0f);
		data[2].uv = Vec2f(1.0f, 1.0f);

		data[3].position = Vec4f(-1.0f, 1.0f, 0.0f, 1.0f);
		data[3].color = Vec4f(1.0f);
		data[3].uv = Vec2f(0.0f, 1.0f);

		std::vector<std::uint32_t> indices = {
			0,
			1,
			2,
			2,
			3,
			0,
		};

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->AddSubMesh({ std::move(data), std::move(indices) });
		return mesh;
	}

	std::shared_ptr<Mesh> CreateCube()
	{
		//    v6----- v5
		//   /|      /|
		//  v1------v0|
		//  | |     | |
		//  | |v7---|-|v4
		//  |/      |/
		//  v2------v3
		std::vector<Vertex> data(24);

		data[0].position = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		data[0].color = Vec4f(1.0f);
		data[0].normal = Vec4f(0.0f, 0.0f, 1.0f, 1.0f);

		data[1].position = Vec4f(-1.0f, 1.0f, 1.0f, 1.0f);
		data[1].color = Vec4f(1.0f);
		data[1].normal = Vec4f(0.0f, 0.0f, 1.0f, 1.0f);

		data[2].position = Vec4f(-1.0f, -1.0f, 1.0f, 1.0f);
		data[2].color = Vec4f(1.0f);
		data[2].normal = Vec4f(0.0f, 0.0f, 1.0f, 1.0f);

		data[3].position = Vec4f(1.0f, -1.0f, 1.0f, 1.0f);
		data[3].color = Vec4f(1.0f);
		data[3].normal = Vec4f(0.0f, 0.0f, 1.0f, 1.0f);

		data[4].position = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		data[4].color = Vec4f(1.0f);
		data[4].normal = Vec4f(1.0f, 0.0f, 0.0f, 1.0f);

		data[5].position = Vec4f(1.0f, -1.0f, 1.0f, 1.0f);
		data[5].color = Vec4f(1.0f);
		data[5].normal = Vec4f(1.0f, 0.0f, 0.0f, 1.0f);

		data[6].position = Vec4f(1.0f, -1.0f, -1.0f, 1.0f);
		data[6].color = Vec4f(1.0f);
		data[6].normal = Vec4f(1.0f, 0.0f, 0.0f, 1.0f);

		data[7].position = Vec4f(1.0f, 1.0f, -1.0f, 1.0f);
		data[7].color = Vec4f(1.0f);
		data[7].normal = Vec4f(1.0f, 0.0f, 0.0f, 1.0f);

		data[8].position = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
		data[8].color = Vec4f(1.0f);
		data[8].normal = Vec4f(0.0f, 1.0f, 0.0f, 1.0f);

		data[9].position = Vec4f(1.0f, 1.0f, -1.0f, 1.0f);
		data[9].color = Vec4f(1.0f);
		data[9].normal = Vec4f(0.0f, 1.0f, 0.0f, 1.0f);

		data[10].position = Vec4f(-1.0f, 1.0f, -1.0f, 1.0f);
		data[10].color = Vec4f(1.0f);
		data[10].normal = Vec4f(0.0f, 1.0f, 0.0f, 1.0f);

		data[11].position = Vec4f(-1.0f, 1.0f, 1.0f, 1.0f);
		data[11].color = Vec4f(1.0f);
		data[11].normal = Vec4f(0.0f, 1.0f, 0.0f, 1.0f);

		data[12].position = Vec4f(-1.0f, 1.0f, 1.0f, 1.0f);
		data[12].color = Vec4f(1.0f);
		data[12].normal = Vec4f(-1.0f, 0.0f, 0.0f, 1.0f);

		data[13].position = Vec4f(-1.0f, 1.0f, -1.0f, 1.0f);
		data[13].color = Vec4f(1.0f);
		data[13].normal = Vec4f(-1.0f, 0.0f, 0.0f, 1.0f);

		data[14].position = Vec4f(-1.0f, -1.0f, -1.0f, 1.0f);
		data[14].color = Vec4f(1.0f);
		data[14].normal = Vec4f(-1.0f, 0.0f, 0.0f, 1.0f);

		data[15].position = Vec4f(-1.0f, -1.0f, 1.0f, 1.0f);
		data[15].color = Vec4f(1.0f);
		data[15].normal = Vec4f(-1.0f, 0.0f, 0.0f, 1.0f);

		data[16].position = Vec4f(-1.0f, -1.0f, -1.0f, 1.0f);
		data[16].color = Vec4f(1.0f);
		data[16].normal = Vec4f(0.0f, -1.0f, 0.0f, 1.0f);

		data[17].position = Vec4f(1.0f, -1.0f, -1.0f, 1.0f);
		data[17].color = Vec4f(1.0f);
		data[17].normal = Vec4f(0.0f, -1.0f, 0.0f, 1.0f);

		data[18].position = Vec4f(1.0f, -1.0f, 1.0f, 1.0f);
		data[18].color = Vec4f(1.0f);
		data[18].normal = Vec4f(0.0f, -1.0f, 0.0f, 1.0f);

		data[19].position = Vec4f(-1.0f, -1.0f, 1.0f, 1.0f);
		data[19].color = Vec4f(1.0f);
		data[19].normal = Vec4f(0.0f, -1.0f, 0.0f, 1.0f);

		data[20].position = Vec4f(1.0f, -1.0f, -1.0f, 1.0f);
		data[20].color = Vec4f(1.0f);
		data[20].normal = Vec4f(0.0f, 0.0f, -1.0f, 1.0f);

		data[21].position = Vec4f(-1.0f, -1.0f, -1.0f, 1.0f);
		data[21].color = Vec4f(1.0f);
		data[21].normal = Vec4f(0.0f, 0.0f, -1.0f, 1.0f);

		data[22].position = Vec4f(-1.0f, 1.0f, -1.0f, 1.0f);
		data[22].color = Vec4f(1.0f);
		data[22].normal = Vec4f(0.0f, 0.0f, -1.0f, 1.0f);

		data[23].position = Vec4f(1.0f, 1.0f, -1.0f, 1.0f);
		data[23].color = Vec4f(1.0f);
		data[23].normal = Vec4f(0.0f, 0.0f, -1.0f, 1.0f);

		for(int i = 0; i < 6; i++)
		{
			data[i * 4 + 0].uv = Vec2f(0.0f, 0.0f);
			data[i * 4 + 1].uv = Vec2f(1.0f, 0.0f);
			data[i * 4 + 2].uv = Vec2f(1.0f, 1.0f);
			data[i * 4 + 3].uv = Vec2f(0.0f, 1.0f);
		}

		std::vector<std::uint32_t> indices = {
			0, 1, 2, 0, 2, 3,       // Right
			4, 5, 6, 4, 6, 7,       // Front
			8, 9, 10, 8, 10, 11,    // Top
			12, 13, 14, 12, 14, 15, // Back
			16, 17, 18, 16, 18, 19, // Bottom
			20, 21, 22, 20, 22, 23  // Left
		};

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->AddSubMesh({ std::move(data), std::move(indices) });
		return mesh;
	}

	std::shared_ptr<Mesh> CreatePyramid()
	{
		std::vector<Vertex> data(18);

		data[0].position = Vec4f(1.0f, 1.0f, -1.0f, 1.0f);
		data[0].color = Vec4f(1.0f);
		data[0].uv = Vec2f(0.24f, 0.20f);
		data[0].normal = Vec4f(0.0f, 0.8948f, 0.4464f, 1.0f);

		data[1].position = Vec4f(-1.0f, 1.0f, -1.0f, 1.0f);
		data[1].color = Vec4f(1.0f);
		data[1].uv = Vec2f(0.24f, 0.81f);
		data[1].normal = Vec4f(0.0f, 0.8948f, 0.4464f, 1.0f);

		data[2].position = Vec4f(0.0f, 0.0f, 1.0f, 1.0f);
		data[2].color = Vec4f(1.0f);
		data[2].uv = Vec2f(0.95f, 0.50f);
		data[2].normal = Vec4f(0.0f, 0.8948f, 0.4464f, 1.0f);

		data[3].position = Vec4f(-1.0f, 1.0f, -1.0f, 1.0f);
		data[3].color = Vec4f(1.0f);
		data[3].uv = Vec2f(0.24f, 0.21f);
		data[3].normal = Vec4f(-0.8948f, 0.0f, 0.4464f, 1.0f);

		data[4].position = Vec4f(-1.0f, -1.0f, -1.0f, 1.0f);
		data[4].color = Vec4f(1.0f);
		data[4].uv = Vec2f(0.24f, 0.81f);
		data[4].normal = Vec4f(-0.8948f, 0.0f, 0.4464f, 1.0f);

		data[5].position = Vec4f(0.0f, 0.0f, 1.0f, 1.0f);
		data[5].color = Vec4f(1.0f);
		data[5].uv = Vec2f(0.95f, 0.50f);
		data[5].normal = Vec4f(-0.8948f, 0.0f, 0.4464f, 1.0f);

		data[6].position = Vec4f(1.0f, 1.0f, -1.0f, 1.0f);
		data[6].color = Vec4f(1.0f);
		data[6].uv = Vec2f(0.24f, 0.81f);
		data[6].normal = Vec4f(0.8948f, 0.0f, 0.4475f, 1.0f);

		data[7].position = Vec4f(0.0f, 0.0f, 1.0f, 1.0f);
		data[7].color = Vec4f(1.0f);
		data[7].uv = Vec2f(0.95f, 0.50f);
		data[7].normal = Vec4f(0.8948f, 0.0f, 0.4475f, 1.0f);

		data[8].position = Vec4f(1.0f, -1.0f, -1.0f, 1.0f);
		data[8].color = Vec4f(1.0f);
		data[8].uv = Vec2f(0.24f, 0.21f);
		data[8].normal = Vec4f(0.8948f, 0.0f, 0.4475f, 1.0f);

		data[9].position = Vec4f(-1.0f, -1.0f, -1.0f, 1.0f);
		data[9].color = Vec4f(1.0f);
		data[9].uv = Vec2f(0.24f, 0.21f);
		data[9].normal = Vec4f(0.0f, -0.8948f, 0.448f, 1.0f);

		data[10].position = Vec4f(1.0f, -1.0f, -1.0f, 1.0f);
		data[10].color = Vec4f(1.0f);
		data[10].uv = Vec2f(0.24f, 0.81f);
		data[10].normal = Vec4f(0.0f, -0.8948f, 0.448f, 1.0f);

		data[11].position = Vec4f(0.0f, 0.0f, 1.0f, 1.0f);
		data[11].color = Vec4f(1.0f);
		data[11].uv = Vec2f(0.95f, 0.50f);
		data[11].normal = Vec4f(0.0f, -0.8948f, 0.448f, 1.0f);

		data[12].position = Vec4f(-1.0f, 1.0f, -1.0f, 1.0f);
		data[12].color = Vec4f(1.0f);
		data[12].uv = Vec2f(0.0f, 0.0f);
		data[12].normal = Vec4f(0.0f, 0.0f, -1.0f, 1.0f);

		data[13].position = Vec4f(1.0f, 1.0f, -1.0f, 1.0f);
		data[13].color = Vec4f(1.0f);
		data[13].uv = Vec2f(0.0f, 1.0f);
		data[13].normal = Vec4f(0.0f, 0.0f, -1.0f, 1.0f);

		data[14].position = Vec4f(1.0f, -1.0f, -1.0f, 1.0f);
		data[14].color = Vec4f(1.0f);
		data[14].uv = Vec2f(1.0f, 1.0f);
		data[14].normal = Vec4f(0.0f, 0.0f, -1.0f, 1.0f);

		data[15].position = Vec4f(-1.0f, -1.0f, -1.0f, 1.0f);
		data[15].color = Vec4f(1.0f);
		data[15].uv = Vec2f(0.96f, 0.0f);
		data[15].normal = Vec4f(0.0f, 0.0f, -1.0f, 1.0f);

		data[16].position = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		data[16].color = Vec4f(1.0f);
		data[16].uv = Vec2f(0.0f, 0.0f);
		data[16].normal = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);

		data[17].position = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		data[17].color = Vec4f(1.0f);
		data[17].uv = Vec2f(0.0f, 0.0f);
		data[17].normal = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);

		std::vector<std::uint32_t> indices = {
			0, 1, 2,
			3, 4, 5,
			6, 7, 8,
			9, 10, 11,
			12, 13, 14,
			15, 12, 14
		};

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->AddSubMesh({ std::move(data), std::move(indices) });
		return mesh;
	}

	std::shared_ptr<Mesh> CreateSphere(std::uint32_t x_segments, std::uint32_t y_segments)
	{
		std::vector<Vertex> data;

		float sector_count = static_cast<float>(x_segments);
		float stack_count = static_cast<float>(y_segments);
		float sector_step = 2 * Pi<float>() / sector_count;
		float stack_step = Pi<float>() / stack_count;
		float radius = 1.0f;

		for(int i = 0; i <= stack_count; ++i)
		{
			float stack_angle = Pi<float>() / 2 - i * stack_step;
			float xy = radius * std::cos(stack_angle);
			float z = radius * std::sin(stack_angle);

			for(int j = 0; j <= sector_count; ++j)
			{
				float sector_angle = j * sector_step;

				float x = xy * cosf(sector_angle);
				float y = xy * sinf(sector_angle);

				float s = static_cast<float>(j / sector_count);
				float t = static_cast<float>(i / stack_count);

				Vertex vertex;
				vertex.position = Vec4f(x, y, z, 1.0f);
				vertex.uv = Vec2f(s, t);
				vertex.normal = vertex.position.Normalize();

				data.emplace_back(vertex);
			}
		}

		std::vector<std::uint32_t> indices;
		std::uint32_t k1, k2;
		for(uint32_t i = 0; i < stack_count; ++i)
		{
			k1 = i * (static_cast<std::uint32_t>(sector_count) + 1U);
			k2 = k1 + static_cast<std::uint32_t>(sector_count) + 1U;

			for(std::uint32_t j = 0; j < sector_count; ++j, ++k1, ++k2)
			{
				if(i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				if(i != (stack_count - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->AddSubMesh({ std::move(data), std::move(indices) });
		return mesh;
	}

	std::shared_ptr<Mesh> CreatePlane(float width, float height, const Vec3f& normal)
	{
		Vec3 vec = normal * 90.0f;

		std::vector<Vertex> data(4);

		data[0].position = Vec4f(-width * 0.5f, -1.0f, -height * 0.5f);
		data[0].normal = normal;
		data[0].uv = Vec2f(0.0f, 0.0f);

		data[1].position = Vec4f(-width * 0.5f, -1.0f, height * 0.5f);
		data[1].normal = normal;
		data[1].uv = Vec2f(0.0f, 1.0f);

		data[2].position = Vec4f(width * 0.5f, 1.0f, height * 0.5f);
		data[2].normal = normal;
		data[2].uv = Vec2f(1.0f, 1.0f);

		data[3].position = Vec4f(width * 0.5f, 1.0f, -height * 0.5f);
		data[3].normal = normal;
		data[3].uv = Vec2f(1.0f, 0.0f);

		std::vector<std::uint32_t> indices = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->AddSubMesh({ std::move(data), std::move(indices) });
		return mesh;
	}

	std::shared_ptr<Mesh> CreateCapsule(float radius, float mid_height, int radial_segments, int rings)
	{
		int i, j, prevrow, thisrow, point;
		float x, y, z, u, v, w;
		float onethird  = 1.0f / 3.0f;
		float twothirds = 2.0f / 3.0f;

		std::vector<Vertex> data;
		std::vector<std::uint32_t> indices;

		point = 0;

		/* top hemisphere */
		thisrow = 0;
		prevrow = 0;
		for(j = 0; j <= (rings + 1); j++)
		{
			v = float(j);

			v /= (rings + 1);
			w = std::sin(0.5f * Pi<float>() * v);
			y = radius * std::cos(0.5f * Pi<float>() * v);

			for(i = 0; i <= radial_segments; i++)
			{
				u = float(i);
				u /= radial_segments;

				x = std::sin(u * (Pi<float>() * 2.0f));
				z = std::cos(u * (Pi<float>() * 2.0f));

				Vec3 p = Vec3f(x * radius * w, y, z * radius * w);

				Vertex vertex;
				vertex.position = Vec4f(p + Vec3f(0.0f, 0.5f * mid_height, 0.0f), 1.0f);
				vertex.normal = p.Normalize();
				vertex.uv = Vec2f(u, onethird * v);
				data.emplace_back(vertex);
				point++;

				if(i > 0 && j > 0)
				{
					indices.push_back(thisrow + i - 1);
					indices.push_back(prevrow + i);
					indices.push_back(prevrow + i - 1);

					indices.push_back(thisrow + i - 1);
					indices.push_back(thisrow + i);
					indices.push_back(prevrow + i);
				};
			};

			prevrow = thisrow;
			thisrow = point;
		};

		/* cylinder */
		thisrow = point;
		prevrow = 0;
		for(j = 0; j <= (rings + 1); j++)
		{
			v = float(j);
			v /= (rings + 1);

			y = mid_height * v;
			y = (mid_height * 0.5f) - y;

			for(i = 0; i <= radial_segments; i++)
			{
				u = float(i);
				u /= radial_segments;

				x = std::sin(u * (Pi<float>() * 2.0f));
				z = std::cos(u * (Pi<float>() * 2.0f));

				Vec3 p = Vec3f(x * radius, y, z * radius);

				Vertex vertex;
				vertex.position = p;
				// vertex.normal = Vec4f(x, z, 0.0f);
				vertex.normal = Vec4f(x, 0.0f, z);
				// vertex.uv = Vec2f(u, onethird + (v * onethird));
				vertex.uv = Vec2f(u, v * 0.5f);
				data.emplace_back(vertex);

				point++;

				if(i > 0 && j > 0)
				{
					indices.push_back(thisrow + i - 1);
					indices.push_back(prevrow + i);
					indices.push_back(prevrow + i - 1);

					indices.push_back(thisrow + i - 1);
					indices.push_back(thisrow + i);
					indices.push_back(prevrow + i);
				};
			};

			prevrow = thisrow;
			thisrow = point;
		};

		/* bottom hemisphere */
		thisrow = point;
		prevrow = 0;

		for(j = 0; j <= (rings + 1); j++)
		{
			v = float(j);

			v /= (rings + 1);
			v += 1.0f;
			w = std::sin(0.5f * Pi<float>() * v);
			y = radius * std::cos(0.5f * Pi<float>() * v);

			for(i = 0; i <= radial_segments; i++)
			{
				float u2 = float(i);
				u2 /= radial_segments;

				x = std::sin(u2 * (Pi<float>() * 2.0f));
				z = std::cos(u2 * (Pi<float>() * 2.0f));

				Vec3 p = Vec3f(x * radius * w, y, z * radius * w);

				Vertex vertex;
				vertex.position = p + Vec3f(0.0f, -0.5f * mid_height, 0.0f);
				vertex.normal = p.Normalize();
				vertex.uv = Vec2f(u2, twothirds + ((v - 1.0f) * onethird));
				data.emplace_back(vertex);

				point++;

				if(i > 0 && j > 0)
				{
					indices.push_back(thisrow + i - 1);
					indices.push_back(prevrow + i);
					indices.push_back(prevrow + i - 1);

					indices.push_back(thisrow + i - 1);
					indices.push_back(thisrow + i);
					indices.push_back(prevrow + i);
				};
			};

			prevrow = thisrow;
			thisrow = point;
		}

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->AddSubMesh({ std::move(data), std::move(indices) });
		return mesh;
	}
}
