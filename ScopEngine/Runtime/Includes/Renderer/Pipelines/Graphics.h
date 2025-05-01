#ifndef __SCOP_GRAPHICS_PIPELINE__
#define __SCOP_GRAPHICS_PIPELINE__

#include <memory>
#include <vector>

#include <kvf.h>

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
		VkCullModeFlagBits culling = VK_CULL_MODE_FRONT_BIT;
		VkPolygonMode mode = VK_POLYGON_MODE_FILL;
		bool no_vertex_inputs = false;
		bool depth_test_equal = false;
		bool clear_color_attachments = true;
	};

	class GraphicPipeline : public Pipeline
	{
		public:
			GraphicPipeline() = default;

			void Init(const GraphicPipelineDescriptor& descriptor);
			bool BindPipeline(VkCommandBuffer command_buffer, std::size_t framebuffer_index, std::array<float, 4> clear) noexcept;
			void EndPipeline(VkCommandBuffer command_buffer) noexcept override;
			void Destroy() noexcept;

			[[nodiscard]] inline VkPipeline GetPipeline() const override { return m_pipeline; }
			[[nodiscard]] inline VkPipelineLayout GetPipelineLayout() const override { return m_pipeline_layout; }
			[[nodiscard]] inline VkPipelineBindPoint GetPipelineBindPoint() const override { return VK_PIPELINE_BIND_POINT_GRAPHICS; }

			~GraphicPipeline() = default;

		private:
			void CreateFramebuffers(const std::vector<NonOwningPtr<Texture>>& render_targets, bool clear_attachments);
			void TransitionAttachments(VkCommandBuffer cmd = VK_NULL_HANDLE);

			// Private override to remove access
			bool BindPipeline(VkCommandBuffer) noexcept override { return false; };

		private:
			std::vector<NonOwningPtr<Texture>> m_attachments;
			std::vector<VkFramebuffer> m_framebuffers;
			std::vector<VkClearValue> m_clears;
			std::shared_ptr<Shader> p_vertex_shader;
			std::shared_ptr<Shader> p_fragment_shader;
			VkRenderPass m_renderpass = VK_NULL_HANDLE;
			VkPipeline m_pipeline = VK_NULL_HANDLE;
			VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
			NonOwningPtr<class Renderer> p_renderer;
			NonOwningPtr<DepthImage> p_depth;
	};
}

#endif
