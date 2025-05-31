#include <Graphics/Text.h>
#include <Renderer/Vertex.h>

#include <vector>

namespace Scop
{
	Text::Text(std::uint64_t uuid, const std::string& text, std::shared_ptr<Font> font) : m_uuid(uuid)
	{
		Assert(font != nullptr, "invalid font");

		std::vector<Vertex> vertex_data;
		std::vector<std::uint32_t> index_data;

		float stb_x = 0.0f;
		float stb_y = 0.0f;

		const auto& char_data = font->GetCharData();
		for(char c : text)
		{
			if(c < 32)
				continue;

			stbtt_aligned_quad q;
			stbtt_GetPackedQuad(char_data.data(), RANGE, RANGE, c - 32, &stb_x, &stb_y, &q, 1);

			std::size_t index = vertex_data.size();

			vertex_data.emplace_back(Vec4f{ q.x0, q.y0, 0.0f, 1.0f }, Vec4f{ 1.0f }, -Vec2f{ q.s0, -q.t0 });
			vertex_data.emplace_back(Vec4f{ q.x1, q.y0, 0.0f, 1.0f }, Vec4f{ 1.0f }, -Vec2f{ q.s1, -q.t0 });
			vertex_data.emplace_back(Vec4f{ q.x1, q.y1, 0.0f, 1.0f }, Vec4f{ 1.0f }, -Vec2f{ q.s1, -q.t1 });
			vertex_data.emplace_back(Vec4f{ q.x0, q.y1, 0.0f, 1.0f }, Vec4f{ 1.0f }, -Vec2f{ q.s0, -q.t1 });

			index_data.push_back(index + 0);
			index_data.push_back(index + 1);
			index_data.push_back(index + 2);
			index_data.push_back(index + 2);
			index_data.push_back(index + 3);
			index_data.push_back(index + 0);
		}

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		mesh->AddSubMesh({ std::move(vertex_data), std::move(index_data) });
		p_mesh = mesh;
		p_font = font;
		m_text = text;
	}
}
