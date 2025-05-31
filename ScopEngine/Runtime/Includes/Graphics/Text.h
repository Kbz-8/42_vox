#ifndef __SCOP_TEXT__
#define __SCOP_TEXT__

#include <Graphics/Font.h>
#include <Graphics/Mesh.h>
#include <Renderer/Descriptor.h>

namespace Scop
{
	class Text
	{
		friend class Render2DPass;

		public:
			Text(std::uint64_t uuid, const std::string& text, std::shared_ptr<Font> font);

			inline void SetColor(Vec4f color) noexcept { m_color = color; }
			inline void SetPosition(Vec2ui position) noexcept { m_position = position; }
			inline void SetScale(Vec2f scale) noexcept { m_scale = scale; }

			[[nodiscard]] inline const std::string& GetText() const { return m_text; }
			[[nodiscard]] inline std::shared_ptr<Font> GetFont() const { return p_font; }
			[[nodiscard]] inline const Vec4f& GetColor() const noexcept { return m_color; }
			[[nodiscard]] inline const Vec2ui& GetPosition() const noexcept { return m_position; }
			[[nodiscard]] inline const Vec2f& GetScale() const noexcept { return m_scale; }
			[[nodiscard]] inline std::shared_ptr<Mesh> GetMesh() const { return p_mesh; }
			[[nodiscard]] inline std::uint64_t GetUUID() const noexcept { return m_uuid; }

			virtual ~Text() = default;

		private:
			[[nodiscard]] inline bool IsSetInit() const noexcept { return m_set.IsInit(); }
			[[nodiscard]] inline VkDescriptorSet GetSet(std::size_t frame_index) const noexcept { return m_set.GetSet(frame_index); }
			inline void UpdateDescriptorSet(const DescriptorSet& set)
			{
				m_set = set.Duplicate();
			}
			inline void Bind(std::size_t frame_index, VkCommandBuffer cmd)
			{
				m_set.SetImage(frame_index, 0, const_cast<Texture&>(p_font->GetTexture()));
				m_set.Update(frame_index, cmd);
			}

		private:
			DescriptorSet m_set;
			std::shared_ptr<Mesh> p_mesh;
			std::shared_ptr<Font> p_font;
			std::string m_text;
			Vec4f m_color = Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f };
			Vec2ui m_position = Vec2ui{ 0, 0 };
			Vec2f m_scale = Vec2f{ 1.0f, 1.0f };
			std::uint64_t m_uuid;
	};
}

#endif
