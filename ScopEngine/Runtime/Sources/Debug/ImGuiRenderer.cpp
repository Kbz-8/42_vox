#include <Debug/ImGuiRenderer.h>
#include <Renderer/Buffer.h>
#include <Renderer/Image.h>
#include <Platform/Inputs.h>
#include <Core/EventBus.h>

#undef DebugLog

#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_vulkan.h>

#include <algorithm>

namespace Scop
{
	std::string HumanSize(uint64_t bytes)
	{
		std::string_view suffix[] = { "B", "KB", "MB", "GB", "TB" };
		std::size_t length = sizeof(suffix) / sizeof(suffix[0]);

		int i = 0;
		double dbl_bytes = bytes;

		if(bytes > 1024)
		{
			for(i = 0; (bytes / 1024) > 0 && i < length-1; i++, bytes /= 1024)
				dbl_bytes = bytes / 1024.0;
		}

		std::string output(256, 0);
		std::sprintf(output.data(), "%.02lf %s", dbl_bytes, suffix[i].data());
		return output;
	}

	ImGuiRenderer::ImGuiRenderer(NonOwningPtr<Renderer> renderer) : p_renderer(renderer)
	{}

	void ImGuiRenderer::Init(Inputs& inputs)
	{
		std::function<void(const EventBase&)> functor = [this](const EventBase& event)
		{
			if(event.What() == Event::ResizeEventCode)
			{
				kvfDestroyRenderPass(RenderCore::Get().GetDevice(), m_renderpass);
				std::vector<VkAttachmentDescription> attachments;
				const Image& image = p_renderer->GetSwapchain().GetSwapchainImages()[0];
				attachments.push_back(kvfBuildAttachmentDescription(KVF_IMAGE_COLOR, image.GetFormat(), image.GetLayout(), image.GetLayout(), false, VK_SAMPLE_COUNT_1_BIT));
				m_renderpass = kvfCreateRenderPass(RenderCore::Get().GetDevice(), attachments.data(), attachments.size(), VK_PIPELINE_BIND_POINT_GRAPHICS);
				CreateFramebuffers();
				ImGui_ImplVulkan_SetMinImageCount(kvfGetSwapchainMinImagesCount(p_renderer->GetSwapchain().Get()));
			}
		};
		EventBus::RegisterListener({ functor, std::to_string((std::uintptr_t)(void**)this) });

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().IniFilename = nullptr;

		SetTheme();

		VkDescriptorPoolSize pool_sizes[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info{};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (std::uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		RenderCore::Get().vkCreateDescriptorPool(RenderCore::Get().GetDevice(), &pool_info, nullptr, &m_pool);

		// Setup Platform/Renderer bindings
		ImGui_ImplVulkan_LoadFunctions([](const char* function_name, void* vulkan_instance) {
			return RenderCore::Get().vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance*>(vulkan_instance)), function_name);
		}, &RenderCore::Get().GetInstanceRef());

		std::vector<VkAttachmentDescription> attachments;
		const Image& image = p_renderer->GetSwapchain().GetSwapchainImages()[0];
		attachments.push_back(kvfBuildAttachmentDescription(KVF_IMAGE_COLOR, image.GetFormat(), image.GetLayout(), image.GetLayout(), false, VK_SAMPLE_COUNT_1_BIT));
		m_renderpass = kvfCreateRenderPass(RenderCore::Get().GetDevice(), attachments.data(), attachments.size(), VK_PIPELINE_BIND_POINT_GRAPHICS);
		CreateFramebuffers();

		ImGui_ImplSDL2_InitForVulkan(p_renderer->GetWindow()->GetSDLWindow());
		ImGui_ImplVulkan_InitInfo init_info{};
			init_info.Instance = RenderCore::Get().GetInstance();
			init_info.PhysicalDevice = RenderCore::Get().GetPhysicalDevice();
			init_info.Device = RenderCore::Get().GetDevice();
			init_info.QueueFamily = kvfGetDeviceQueueFamily(RenderCore::Get().GetDevice(), KVF_GRAPHICS_QUEUE);
			init_info.Queue = kvfGetDeviceQueue(RenderCore::Get().GetDevice(), KVF_GRAPHICS_QUEUE);
			init_info.DescriptorPool = m_pool;
			init_info.Allocator = nullptr;
			init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			init_info.Subpass = 0;
			init_info.MinImageCount = kvfGetSwapchainMinImagesCount(p_renderer->GetSwapchain().Get());
			init_info.ImageCount = p_renderer->GetSwapchain().GetSwapchainImages().size();
			init_info.CheckVkResultFn = [](VkResult result){ kvfCheckVk(result); };
			init_info.RenderPass = m_renderpass;
		ImGui_ImplVulkan_Init(&init_info);

		inputs.AddEventUpdateHook(ImGui_ImplSDL2_ProcessEvent);
	}

	void ImGuiRenderer::Destroy()
	{
		RenderCore::Get().WaitDeviceIdle();
		for(VkFramebuffer fb : m_framebuffers)
			kvfDestroyFramebuffer(RenderCore::Get().GetDevice(), fb);
		m_framebuffers.clear();
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		kvfDestroyRenderPass(RenderCore::Get().GetDevice(), m_renderpass);
		RenderCore::Get().vkDestroyDescriptorPool(RenderCore::Get().GetDevice(), m_pool, nullptr);
	}

	bool ImGuiRenderer::BeginFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		return true;
	}

	void ImGuiRenderer::DisplayRenderStatistics()
	{
		static std::array<std::string, 5> gpu_types_string = {
			"Other",
			"Integrated",
			"Graphics Card",
			"Virtual",
			"CPU"
		};
		static std::array<float, 1000> frame_histogram;

		float delta = ImGui::GetIO().DeltaTime;
		ImGui::SetNextWindowPos(ImVec2{ 20.0f, 20.0f }, ImGuiCond_FirstUseEver);
		if(ImGui::Begin("Render Statistics", nullptr, ImGuiWindowFlags_NoResize))
		{
			VkPhysicalDeviceProperties props;
			RenderCore::Get().vkGetPhysicalDeviceProperties(RenderCore::Get().GetPhysicalDevice(), &props);
			ImGui::Text("GPU in use:\n%s", props.deviceName);
			ImGui::Text("GPU type: %s", gpu_types_string[static_cast<int>(props.deviceType)].c_str());
			ImGui::Separator();
			ImGui::Text("Frame time %.3fms", delta);
			ImGui::PlotLines("##Frames_Histogram", frame_histogram.data(), frame_histogram.size(), 0, nullptr, 0.0f, 1.0f, ImVec2(ImGui::GetContentRegionAvail().x, 40.0f));
			ImGui::Text("FPS %.0f", ImGui::GetIO().Framerate);
			ImGui::Separator();
			ImGui::Text("Swapchain images count %ld", p_renderer->GetSwapchain().GetSwapchainImages().size());
			ImGui::Text("Drawcalls %ld", p_renderer->GetDrawCallsCounterRef());
			ImGui::Text("Polygon drawn %ld", p_renderer->GetPolygonDrawnCounterRef());
			ImGui::Separator();
			ImGui::Text("VRAM usage %s", HumanSize(RenderCore::Get().GetAllocator().GetVramUsage()).c_str());
			ImGui::Text("Host visible usage %s", HumanSize(RenderCore::Get().GetAllocator().GetVramHostVisibleUsage()).c_str());
			ImGui::Text("Allocations count %ld / %u", RenderCore::Get().GetAllocator().GetAllocationsCount(), props.limits.maxMemoryAllocationCount);
			ImGui::Text("Buffer count %ld", GPUBuffer::GetBufferCount());
			ImGui::Text("Image count %ld", Image::GetImageCount());
			ImGui::Separator();
			ImGui::Text("Window dimensions: %ux%u", p_renderer->GetWindow()->GetWidth(), p_renderer->GetWindow()->GetHeight());
		}
		ImGui::End();
		std::rotate(frame_histogram.begin(), frame_histogram.begin() + 1, frame_histogram.end());
		frame_histogram.back() = delta;
	}

	void ImGuiRenderer::EndFrame()
	{
		VkFramebuffer fb = m_framebuffers[p_renderer->GetSwapchain().GetImageIndex()];
		ImGui::Render();
		ImDrawData* draw_data = ImGui::GetDrawData();
		if(draw_data->DisplaySize.x >= 0.0f && draw_data->DisplaySize.y >= 0.0f)
		{
			VkExtent2D fb_extent = kvfGetFramebufferSize(fb);
			kvfBeginRenderPass(m_renderpass, p_renderer->GetActiveCommandBuffer(), fb, fb_extent, nullptr, 0);
			ImGui_ImplVulkan_RenderDrawData(draw_data, p_renderer->GetActiveCommandBuffer());
			RenderCore::Get().vkCmdEndRenderPass(p_renderer->GetActiveCommandBuffer());
		}
	}

	void ImGuiRenderer::CreateFramebuffers()
	{
		for(VkFramebuffer fb : m_framebuffers)
			kvfDestroyFramebuffer(RenderCore::Get().GetDevice(), fb);
		m_framebuffers.clear();
		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkImageView> attachment_views;
		const Image& image = p_renderer->GetSwapchain().GetSwapchainImages()[0];
		attachments.push_back(kvfBuildAttachmentDescription((kvfIsDepthFormat(image.GetFormat()) ? KVF_IMAGE_DEPTH : KVF_IMAGE_COLOR), image.GetFormat(), image.GetLayout(), image.GetLayout(), false, VK_SAMPLE_COUNT_1_BIT));
		attachment_views.push_back(image.GetImageView());
		for(const Image& image : p_renderer->GetSwapchain().GetSwapchainImages())
		{
			attachment_views[0] = image.GetImageView();
			m_framebuffers.push_back(kvfCreateFramebuffer(RenderCore::Get().GetDevice(), m_renderpass, attachment_views.data(), attachment_views.size(), { .width = image.GetWidth(), .height = image.GetHeight() }));
		}
	}

	void ImGuiRenderer::SetTheme()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		ImGui::StyleColorsDark();
		colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg]              = ImVec4(0.10f, 0.10f, 0.10f, 0.50f);
		colors[ImGuiCol_ChildBg]               = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
		colors[ImGuiCol_PopupBg]               = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border]                = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_BorderShadow]          = ImVec4(0.05f, 0.05f, 0.05f, 0.24f);
		colors[ImGuiCol_FrameBg]               = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg]               = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_TitleBgActive]         = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_MenuBarBg]             = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark]             = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab]            = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header]                = ImVec4(0.05f, 0.05f, 0.05f, 0.52f);
		colors[ImGuiCol_HeaderHovered]         = ImVec4(0.05f, 0.05f, 0.05f, 0.36f);
		colors[ImGuiCol_HeaderActive]          = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator]             = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip]            = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab]                   = ImVec4(0.05f, 0.05f, 0.05f, 0.52f);
		colors[ImGuiCol_TabHovered]            = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive]             = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_TabUnfocused]          = ImVec4(0.05f, 0.05f, 0.05f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_PlotLines]             = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.05f, 0.05f, 0.05f, 0.52f);
		colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.05f, 0.05f, 0.05f, 0.52f);
		colors[ImGuiCol_TableBorderLight]      = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget]        = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		style->ChildRounding = 4.0f;
		style->FrameBorderSize = 1.0f;
		style->FrameRounding = 4.0f;
		style->GrabMinSize = 7.0f;
		style->PopupRounding = 2.0f;
		style->ScrollbarRounding = 12.0f;
		style->ScrollbarSize = 13.0f;
		style->TabBorderSize = 0.0f;
		style->TabRounding = 5.0f;
		style->WindowRounding = 0.0f;
		style->WindowBorderSize = 1.0f;
		style->AntiAliasedLines = true;
		style->AntiAliasedFill = true;
		style->TabBorderSize = 2.0f;
	}
}
