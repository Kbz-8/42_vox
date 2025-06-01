#ifndef __SCOP_SCENE__
#define __SCOP_SCENE__

#include <memory>
#include <string>
#include <string_view>

#include <Utils/NonOwningPtr.h>

#include <Graphics/Enums.h>
#include <Graphics/Actor.h>
#include <Graphics/Narrator.h>
#include <Graphics/Sprite.h>
#include <Renderer/Buffer.h>
#include <Renderer/Descriptor.h>
#include <Renderer/RenderCore.h>
#include <Graphics/Cameras/Base.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/Pipelines/Graphics.h>
#include <Graphics/Font.h>
#include <Graphics/Text.h>

namespace Scop
{
	struct SceneDescriptor
	{
		std::shared_ptr<Shader> fragment_shader;
		std::shared_ptr<Shader> post_process_shader = nullptr;
		std::shared_ptr<BaseCamera> camera;
		CullMode culling;
		std::size_t post_process_data_size = 0;
		bool render_3D_enabled = true;
		bool render_2D_enabled = true;
		bool render_skybox_enabled = true;
		bool render_post_process_enabled = false;
	};

	class Scene
	{
		friend class ScopEngine;

		public:
			struct ForwardData
			{
				std::shared_ptr<DescriptorSet> matrices_set;
				std::shared_ptr<DescriptorSet> albedo_set;
				std::shared_ptr<UniformBuffer> matrices_buffer;
				bool wireframe = false;
			};

			struct PostProcessData
			{
				std::shared_ptr<DescriptorSet> set;
				std::shared_ptr<UniformBuffer> data_buffer;
				CPUBuffer data;
			};

		public:
			Scene(std::string_view name, SceneDescriptor desc);
			Scene(std::string_view name, SceneDescriptor desc, NonOwningPtr<Scene> parent);

			Actor& CreateActor(Model model) noexcept;
			Actor& CreateActor(std::string_view name, Model model);

			Narrator& CreateNarrator() noexcept;
			Narrator& CreateNarrator(std::string_view name);

			Sprite& CreateSprite(std::shared_ptr<Texture> texture) noexcept;
			Sprite& CreateSprite(std::string_view name, std::shared_ptr<Texture> texture);

			Text& CreateText(std::string text) noexcept;
			Text& CreateText(std::string_view name, std::string text);

			void LoadFont(std::filesystem::path path, float scale);

			void RemoveActor(Actor& actor) noexcept;
			void RemoveNarrator(Narrator& narrator) noexcept;
			void RemoveSprite(Sprite& sprite) noexcept;
			void RemoveText(Text& text) noexcept;

			[[nodiscard]] inline Scene& AddChildScene(std::string_view name, SceneDescriptor desc) { return m_scene_children.emplace_back(name, std::move(desc), this); }
			inline void AddSkybox(std::shared_ptr<CubeTexture> cubemap) { p_skybox = cubemap; }
			void SwitchToChild(std::string_view name) const noexcept;
			void SwitchToParent() const noexcept;

			[[nodiscard]] inline ForwardData& GetForwardData() noexcept { return m_forward; }
			[[nodiscard]] inline PostProcessData& GetPostProcessData() noexcept { return m_post_process; }
			[[nodiscard]] inline const std::unordered_map<std::uint64_t, Actor>& GetActors() const noexcept { return m_actors; }
			[[nodiscard]] inline const std::unordered_map<std::uint64_t, Sprite>& GetSprites() const noexcept { return m_sprites; }
			[[nodiscard]] inline const std::unordered_map<std::uint64_t, Text>& GetTexts() const noexcept { return m_texts; }
			[[nodiscard]] inline const std::string& GetName() const noexcept { return m_name; }
			[[nodiscard]] inline GraphicPipeline& GetPipeline() noexcept { return m_pipeline; }
			[[nodiscard]] inline std::shared_ptr<BaseCamera> GetCamera() const { return m_descriptor.camera; }
			[[nodiscard]] inline DepthImage& GetDepth() noexcept { return m_depth; }
			[[nodiscard]] inline std::shared_ptr<Shader> GetFragmentShader() const { return m_descriptor.fragment_shader; }
			[[nodiscard]] inline std::shared_ptr<CubeTexture> GetSkybox() const { return p_skybox; }
			[[nodiscard]] inline const SceneDescriptor& GetDescription() const noexcept { return m_descriptor; }

			~Scene() = default;

		private:
			Scene() = default;
			void Init(NonOwningPtr<class Renderer> renderer);
			void Update(class Inputs& input, float delta, float aspect);
			void Destroy();

		private:
			GraphicPipeline m_pipeline;
			ForwardData m_forward;
			PostProcessData m_post_process;
			DepthImage m_depth;
			SceneDescriptor m_descriptor;
			FontRegistry m_fonts_registry;
			std::shared_ptr<CubeTexture> p_skybox;
			std::unordered_map<std::uint64_t, Actor> m_actors;
			std::unordered_map<std::uint64_t, Text> m_texts;
			std::unordered_map<std::uint64_t, Sprite> m_sprites;
			std::unordered_map<std::uint64_t, Narrator> m_narrators;
			std::vector<Scene> m_scene_children;
			std::string m_name;
			NonOwningPtr<Scene> p_parent;
			std::shared_ptr<Font> p_bound_font;
	};
}

#endif
