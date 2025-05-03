#include <Renderer/Pipelines/Graphics.h>
#include <Renderer/RenderCore.h>
#include <Renderer/Renderer.h>
#include <Renderer/Vertex.h>
#include <Core/EventBus.h>
#include <Core/Logs.h>

namespace Scop
{
	void GraphicPipeline::Init(const GraphicPipelineDescriptor& descriptor)
	{
		if(!descriptor.vertex_shader || !descriptor.fragment_shader)
			FatalError("Vulkan: invalid shaders");

		m_attachments = descriptor.color_attachments;
		p_vertex_shader = descriptor.vertex_shader;
		p_fragment_shader = descriptor.fragment_shader;
		p_renderer = descriptor.renderer;
		p_depth = descriptor.depth;

		m_name = descriptor.name;
		std::cout << m_name << std::endl;

		std::vector<VkPushConstantRange> push_constants;
		std::vector<VkDescriptorSetLayout> set_layouts;
		push_constants.insert(push_constants.end(), p_vertex_shader->GetPipelineLayout().push_constants.begin(), p_vertex_shader->GetPipelineLayout().push_constants.end());
		push_constants.insert(push_constants.end(), p_fragment_shader->GetPipelineLayout().push_constants.begin(), p_fragment_shader->GetPipelineLayout().push_constants.end());
		set_layouts.insert(set_layouts.end(), p_vertex_shader->GetPipelineLayout().set_layouts.begin(), p_vertex_shader->GetPipelineLayout().set_layouts.end());
		set_layouts.insert(set_layouts.end(), p_fragment_shader->GetPipelineLayout().set_layouts.begin(), p_fragment_shader->GetPipelineLayout().set_layouts.end());
		m_pipeline_layout = kvfCreatePipelineLayout(RenderCore::Get().GetDevice(), set_layouts.data(), set_layouts.size(), push_constants.data(), push_constants.size());

		CreateFramebuffers(m_attachments, descriptor.clear_color_attachments);

		VkPhysicalDeviceFeatures features{};
		RenderCore::Get().vkGetPhysicalDeviceFeatures(RenderCore::Get().GetPhysicalDevice(), &features);

		KvfGraphicsPipelineBuilder* builder = kvfCreateGPipelineBuilder();
		kvfGPipelineBuilderAddShaderStage(builder, p_vertex_shader->GetShaderStage(), p_vertex_shader->GetShaderModule(), "main");
		kvfGPipelineBuilderAddShaderStage(builder, p_fragment_shader->GetShaderStage(), p_fragment_shader->GetShaderModule(), "main");
		kvfGPipelineBuilderSetInputTopology(builder, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		kvfGPipelineBuilderSetCullMode(builder, descriptor.culling, VK_FRONT_FACE_CLOCKWISE);
		kvfGPipelineBuilderEnableAlphaBlending(builder);
		if(p_depth)
			kvfGPipelineBuilderEnableDepthTest(builder, (descriptor.depth_test_equal ? VK_COMPARE_OP_EQUAL : VK_COMPARE_OP_LESS), true);
		else
			kvfGPipelineBuilderDisableDepthTest(builder);
		if(features.fillModeNonSolid)
			kvfGPipelineBuilderSetPolygonMode(builder, descriptor.mode, 1.0f);
		else
			kvfGPipelineBuilderSetPolygonMode(builder, VK_POLYGON_MODE_FILL, 1.0f);
		if(features.sampleRateShading)
			kvfGPipelineBuilderSetMultisamplingShading(builder, VK_SAMPLE_COUNT_1_BIT, 0.25f);
		else
			kvfGPipelineBuilderSetMultisampling(builder, VK_SAMPLE_COUNT_1_BIT);

		if(!descriptor.no_vertex_inputs)
		{
			VkVertexInputBindingDescription binding_description = Vertex::GetBindingDescription();
			auto attributes_description = Vertex::GetAttributeDescriptions();
			kvfGPipelineBuilderSetVertexInputs(builder, binding_description, attributes_description.data(), attributes_description.size());
		}

		m_pipeline = kvfCreateGraphicsPipeline(RenderCore::Get().GetDevice(), VK_NULL_HANDLE, m_pipeline_layout, builder, m_renderpass);
		kvfDestroyGPipelineBuilder(builder);

		#ifdef SCOP_HAS_DEBUG_UTILS_FUNCTIONS
			VkDebugUtilsObjectNameInfoEXT name_info{};
			name_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			name_info.objectType = VK_OBJECT_TYPE_PIPELINE;
			name_info.objectHandle = reinterpret_cast<std::uint64_t>(m_pipeline);
			name_info.pObjectName = descriptor.name.data();
			RenderCore::Get().vkSetDebugUtilsObjectNameEXT(RenderCore::Get().GetDevice(), &name_info);

			name_info.objectType = VK_OBJECT_TYPE_RENDER_PASS;
			name_info.objectHandle = reinterpret_cast<std::uint64_t>(m_renderpass);
			RenderCore::Get().vkSetDebugUtilsObjectNameEXT(RenderCore::Get().GetDevice(), &name_info);

			name_info.objectType = VK_OBJECT_TYPE_SHADER_MODULE;
			name_info.objectHandle = reinterpret_cast<std::uint64_t>(p_vertex_shader->GetShaderModule());
			RenderCore::Get().vkSetDebugUtilsObjectNameEXT(RenderCore::Get().GetDevice(), &name_info);

			name_info.objectHandle = reinterpret_cast<std::uint64_t>(p_fragment_shader->GetShaderModule());
			RenderCore::Get().vkSetDebugUtilsObjectNameEXT(RenderCore::Get().GetDevice(), &name_info);

			name_info.objectType = VK_OBJECT_TYPE_FRAMEBUFFER;
			for(VkFramebuffer fb : m_framebuffers)
			{
				name_info.objectHandle = reinterpret_cast<std::uint64_t>(fb);
				RenderCore::Get().vkSetDebugUtilsObjectNameEXT(RenderCore::Get().GetDevice(), &name_info);
			}
			Message("Vulkan: % graphics pipeline created", descriptor.name);
		#else
			Message("Vulkan: graphics pipeline created");
		#endif
	}

	bool GraphicPipeline::BindPipeline(VkCommandBuffer command_buffer, std::size_t framebuffer_index, std::array<float, 4> clear) noexcept
	{
		TransitionAttachments(command_buffer);

		VkFramebuffer fb = m_framebuffers[framebuffer_index];
		VkExtent2D fb_extent = kvfGetFramebufferSize(fb);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = fb_extent.width;
		viewport.height = fb_extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		RenderCore::Get().vkCmdSetViewport(command_buffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = fb_extent;
		RenderCore::Get().vkCmdSetScissor(command_buffer, 0, 1, &scissor);

		for(int i = 0; i < m_clears.size(); i++)
		{
			m_clears[i].color.float32[0] = clear[0];
			m_clears[i].color.float32[1] = clear[1];
			m_clears[i].color.float32[2] = clear[2];
			m_clears[i].color.float32[3] = clear[3];
		}

		if(p_depth)
			m_clears.back().depthStencil = VkClearDepthStencilValue{ 1.0f, 0 };

		kvfBeginRenderPass(m_renderpass, command_buffer, fb, fb_extent, m_clears.data(), m_clears.size());
		RenderCore::Get().vkCmdBindPipeline(command_buffer, GetPipelineBindPoint(), GetPipeline());
		return true;
	}

	void GraphicPipeline::EndPipeline(VkCommandBuffer command_buffer) noexcept
	{
		RenderCore::Get().vkCmdEndRenderPass(command_buffer);
	}

	void GraphicPipeline::Destroy() noexcept
	{
		if(m_pipeline == VK_NULL_HANDLE)
			return;
		std::cout << m_name << std::endl;
		RenderCore::Get().WaitDeviceIdle();

		for(auto& fb : m_framebuffers)
		{
			kvfDestroyFramebuffer(RenderCore::Get().GetDevice(), fb);
			Message("Vulkan: framebuffer destroyed");
		}

		kvfDestroyPipelineLayout(RenderCore::Get().GetDevice(), m_pipeline_layout);
		Message("Vulkan: graphics pipeline layout destroyed");
		kvfDestroyRenderPass(RenderCore::Get().GetDevice(), m_renderpass);
		Message("Vulkan: renderpass destroyed");
		kvfDestroyPipeline(RenderCore::Get().GetDevice(), m_pipeline);

		p_vertex_shader.reset();
		p_fragment_shader.reset();
		m_attachments.clear();
		m_framebuffers.clear();
		m_clears.clear();
		m_renderpass = VK_NULL_HANDLE;
		m_pipeline = VK_NULL_HANDLE;
		m_pipeline_layout = VK_NULL_HANDLE;
		p_renderer = nullptr;
		p_depth = nullptr;
		Message("Vulkan: graphics pipeline destroyed");
	}

	void GraphicPipeline::CreateFramebuffers(const std::vector<NonOwningPtr<Texture>>& render_targets, bool clear_attachments)
	{
		TransitionAttachments();

		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkImageView> attachment_views;
		if(p_renderer)
		{
			attachments.push_back(kvfBuildSwapchainAttachmentDescription(p_renderer->GetSwapchain().Get(), clear_attachments));
			attachment_views.push_back(p_renderer->GetSwapchain().GetSwapchainImages()[0].GetImageView());
		}

		for(NonOwningPtr<Texture> image : render_targets)
		{
			attachments.push_back(kvfBuildAttachmentDescription(KVF_IMAGE_COLOR, image->GetFormat(), image->GetLayout(), image->GetLayout(), clear_attachments, VK_SAMPLE_COUNT_1_BIT));
			attachment_views.push_back(image->GetImageView());
		}

		if(p_depth)
		{
			attachments.push_back(kvfBuildAttachmentDescription(KVF_IMAGE_DEPTH, p_depth->GetFormat(), p_depth->GetLayout(), p_depth->GetLayout(), clear_attachments, VK_SAMPLE_COUNT_1_BIT));
			attachment_views.push_back(p_depth->GetImageView());
		}

		m_renderpass = kvfCreateRenderPass(RenderCore::Get().GetDevice(), attachments.data(), attachments.size(), GetPipelineBindPoint());
		m_clears.clear();
		m_clears.resize(attachments.size());
		Message("Vulkan: renderpass created");

		if(p_renderer)
		{
			for(const Image& image : p_renderer->GetSwapchain().GetSwapchainImages())
			{
				attachment_views[0] = image.GetImageView();
				m_framebuffers.push_back(kvfCreateFramebuffer(RenderCore::Get().GetDevice(), m_renderpass, attachment_views.data(), attachment_views.size(), { .width = image.GetWidth(), .height = image.GetHeight() }));
				Message("Vulkan: framebuffer created");
			}
		}
		for(NonOwningPtr<Texture> image : render_targets)
		{
			m_framebuffers.push_back(kvfCreateFramebuffer(RenderCore::Get().GetDevice(), m_renderpass, attachment_views.data(), attachment_views.size(), { .width = image->GetWidth(), .height = image->GetHeight() }));
			Message("Vulkan: framebuffer created");
		}
	}

	void GraphicPipeline::TransitionAttachments(VkCommandBuffer cmd)
	{
		if(p_depth)
			p_depth->TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, cmd);

		for(NonOwningPtr<Texture> image : m_attachments)
			image->TransitionLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, cmd);
	}
}
