#ifndef __SCOP_GRAPHICS_PIPELINE__
#define __SCOP_GRAPHICS_PIPELINE__

#include <memory>
#include <vector>

#include <kvf.h>

#include <Graphics/Enums.h>
#include <Renderer/Image.h>
#include <Utils/NonOwningPtr.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/Pipelines/Pipeline.h>

namespace Scop
{
	struct GraphicPipelineDescriptor
	{
		std::shared_ptr<Shader> vertex_shader;
		std::shared_ptr<Shader> fragment_shader;
		std::vector<NonOwningPtr<Texture>> color_attachments;
		NonOwningPtr<DepthImage> depth = nullptr;
		NonOwningPtr<class Renderer> renderer = nullptr;
		std::string name = {};
		CullMode culling = CullMode::Front;
		bool no_vertex_inputs = false;
		bool depth_test_equal = false;
		bool clear_color_attachments = true;
		bool wireframe = false;
	};

	class GraphicPipeline : public Pipeline
	{
		friend class Render2DPass;
		friend class FinalPass;
		friend class ForwardPass;
		friend class PostProcessPass;
		friend class SkyboxPass;

		public:
			GraphicPipeline() = default;

			inline void Setup(GraphicPipelineDescriptor descriptor)
			{
				if(!descriptor.vertex_shader || !descriptor.fragment_shader)
					FatalError("Vulkan: invalid shaders");
				m_description = std::move(descriptor);
			}

			bool BindPipeline(VkCommandBuffer command_buffer, std::size_t framebuffer_index, std::array<float, 4> clear) noexcept;
			void EndPipeline(VkCommandBuffer command_buffer) noexcept override;
			void Destroy() noexcept;

			[[nodiscard]] inline VkPipeline GetPipeline() const override { return m_pipeline; }
			[[nodiscard]] inline VkPipelineLayout GetPipelineLayout() const override { return m_pipeline_layout; }
			[[nodiscard]] inline VkPipelineBindPoint GetPipelineBindPoint() const override { return VK_PIPELINE_BIND_POINT_GRAPHICS; }
			[[nodiscard]] inline bool IsPipelineBound() const noexcept { return s_bound_pipeline == this; }
			[[nodiscard]] inline GraphicPipelineDescriptor& GetDescription() noexcept { return m_description; }

			inline ~GraphicPipeline() noexcept { Destroy(); }

		private:
			void Init(GraphicPipelineDescriptor descriptor);
			void CreateFramebuffers(const std::vector<NonOwningPtr<Texture>>& render_targets, bool clear_attachments);
			void TransitionAttachments(VkCommandBuffer cmd = VK_NULL_HANDLE);

			// Private override to remove access
			bool BindPipeline(VkCommandBuffer) noexcept override { return false; };

		private:
			static inline GraphicPipeline* s_bound_pipeline = nullptr;

			GraphicPipelineDescriptor m_description;
			std::vector<VkFramebuffer> m_framebuffers;
			std::vector<VkClearValue> m_clears;
			VkRenderPass m_renderpass = VK_NULL_HANDLE;
			VkPipeline m_pipeline = VK_NULL_HANDLE;
			VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
	};
}

#endif
