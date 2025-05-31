#include <Graphics/Font.h>
#include <Core/Logs.h>

#include <fstream>

#define STBRP_ASSERT(x) Scop::Assert(x, "internal stb assertion " #x)
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

namespace Scop
{
	void Font::BuildFont()
	{
		std::vector<std::uint8_t> file_bytes;
		if(std::holds_alternative<std::filesystem::path>(m_build_data))
		{
			std::ifstream file(std::get<std::filesystem::path>(m_build_data), std::ios::binary);
			if(!file.is_open())
			{
				Error("Font: cannot open font file, %", m_name);
				return;
			}
			std::ifstream::pos_type file_size = std::filesystem::file_size(std::get<std::filesystem::path>(m_build_data));
			file.seekg(0, std::ios::beg);
			file_bytes.resize(file_size);
			file.read(reinterpret_cast<char*>(file_bytes.data()), file_size);
			file.close();
		}

		CPUBuffer bitmap(RANGE * RANGE);

		stbtt_pack_context pc;
		stbtt_PackBegin(&pc, bitmap.GetData(), RANGE, RANGE, RANGE, 1, nullptr);
		if(std::holds_alternative<std::filesystem::path>(m_build_data))
			stbtt_PackFontRange(&pc, file_bytes.data(), 0, m_scale, 32, 96, m_cdata.data());
		else
			stbtt_PackFontRange(&pc, std::get<std::vector<std::uint8_t>>(m_build_data).data(), 0, m_scale, 32, 96, m_cdata.data());
		stbtt_PackEnd(&pc);

		// TODO : find better solution; No, using VK_FORMAT_R8_SRGB does not work
		CPUBuffer vulkan_bitmap(RANGE * RANGE * 4);
		for(int i = 0, j = 0; i < RANGE * RANGE; i++, j += 4)
		{
			vulkan_bitmap.GetData()[j + 0] = bitmap.GetData()[i];
			vulkan_bitmap.GetData()[j + 1] = bitmap.GetData()[i];
			vulkan_bitmap.GetData()[j + 2] = bitmap.GetData()[i];
			vulkan_bitmap.GetData()[j + 3] = bitmap.GetData()[i];
		}

		m_atlas.Init(std::move(vulkan_bitmap), RANGE, RANGE, VK_FORMAT_R8G8B8A8_SRGB, false, m_name + "_font_atlas_" + std::to_string(m_scale));

		Message("Font: loaded % with a scale of %", m_name, m_scale);
	}

	void Font::Destroy()
	{
		if(!m_atlas.IsInit())
			return;
		m_atlas.Destroy();
		Message("Font: unloaded % with a scale of %", m_name, m_scale);
	}
}
