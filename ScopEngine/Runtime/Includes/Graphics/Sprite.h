#ifndef __SCOP_RENDERER_SPRITE__
#define __SCOP_RENDERER_SPRITE__

#include <memory>

#include <Maths/Vec2.h>
#include <Maths/Vec4.h>
#include <Core/Script.h>
#include <Graphics/Mesh.h>
#include <Renderer/Descriptor.h>
#include <Renderer/Image.h>

namespace Scop
{
	class Sprite
	{
		friend class Render2DPass;

		public:
			Sprite(std::shared_ptr<Texture> texture);

			inline void AttachScript(std::shared_ptr<SpriteScript> script) { p_script = script; }
			void Update(NonOwningPtr<class Scene> scene, class Inputs& input, float timestep);

			inline void SetColor(Vec4f color) noexcept { m_color = color; }
			inline void SetPosition(Vec2ui position) noexcept { m_position = position; }
			inline void SetScale(Vec2f scale) noexcept { m_scale = scale; }

			[[nodiscard]] inline const Vec4f& GetColor() const noexcept { return m_color; }
			[[nodiscard]] inline const Vec2ui& GetPosition() const noexcept { return m_position; }
			[[nodiscard]] inline const Vec2f& GetScale() const noexcept { return m_scale; }
			[[nodiscard]] inline std::shared_ptr<Mesh> GetMesh() const { return p_mesh; }
			[[nodiscard]] inline std::shared_ptr<Texture> GetTexture() const { return p_texture; }
			[[nodiscard]] inline std::uint32_t GetUUID() const noexcept { return m_uuid; }

			~Sprite();

		private:
			[[nodiscard]] inline bool IsSetInit() const noexcept { return m_set.IsInit(); }
			[[nodiscard]] inline VkDescriptorSet GetSet(std::size_t frame_index) const noexcept { return m_set.GetSet(frame_index); }

			inline void UpdateDescriptorSet(const DescriptorSet& set)
			{
				m_set = set.Duplicate();
			}

			inline void Bind(std::size_t frame_index, VkCommandBuffer cmd)
			{
				m_set.SetImage(frame_index, 0, *p_texture);
				m_set.Update(frame_index, cmd);
			}

		private:
			DescriptorSet m_set;
			std::shared_ptr<Texture> p_texture;
			std::shared_ptr<class SpriteScript> p_script;
			std::shared_ptr<Mesh> p_mesh;
			Vec4f m_color = Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f };
			Vec2ui m_position = Vec2ui{ 0, 0 };
			Vec2f m_scale = Vec2f{ 1.0f, 1.0f };
			std::uint64_t m_uuid;
	};
}

#endif
