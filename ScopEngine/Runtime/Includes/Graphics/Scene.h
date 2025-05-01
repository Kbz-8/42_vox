#ifndef __SCOP_SCENE__
#define __SCOP_SCENE__

#include <memory>
#include <string>
#include <string_view>

#include <Utils/NonOwningPtr.h>

#include <Graphics/Actor.h>
#include <Graphics/Sprite.h>
#include <Renderer/Buffer.h>
#include <Renderer/Descriptor.h>
#include <Renderer/RenderCore.h>
#include <Graphics/Cameras/Base.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/Pipelines/Graphics.h>

namespace Scop
{
	struct SceneDescriptor
	{
		std::shared_ptr<Shader> fragment_shader;
		std::shared_ptr<BaseCamera> camera;
		bool render_3D_enabled = true;
		bool render_2D_enabled = true;
		bool render_skybox_enabled = true;
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

		public:
			Scene(std::string_view name, SceneDescriptor desc);
			Scene(std::string_view name, SceneDescriptor desc, NonOwningPtr<Scene> parent);

			Actor& CreateActor(Model model) noexcept;
			Actor& CreateActor(std::string_view name, Model model);

			Sprite& CreateSprite(std::shared_ptr<Texture> texture) noexcept;
			Sprite& CreateSprite(std::string_view name, std::shared_ptr<Texture> texture);

			[[nodiscard]] inline Scene& AddChildScene(std::string_view name, SceneDescriptor desc) { return m_scene_children.emplace_back(name, std::move(desc), this); }
			inline void AddSkybox(std::shared_ptr<CubeTexture> cubemap) { p_skybox = cubemap; }
			void SwitchToChild(std::string_view name) const noexcept;
			void SwitchToParent() const noexcept;

			[[nodiscard]] inline ForwardData& GetForwardData() noexcept { return m_forward; }
			[[nodiscard]] inline const std::vector<std::shared_ptr<Actor>>& GetActors() const noexcept { return m_actors; }
			[[nodiscard]] inline const std::vector<std::shared_ptr<Sprite>>& GetSprites() const noexcept { return m_sprites; }
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
			DepthImage m_depth;
			SceneDescriptor m_descriptor;
			std::shared_ptr<CubeTexture> p_skybox;
			std::vector<std::shared_ptr<Actor>> m_actors;
			std::vector<std::shared_ptr<Sprite>> m_sprites;
			std::vector<Scene> m_scene_children;
			std::string m_name;
			NonOwningPtr<Scene> p_parent;
	};
}

#endif
