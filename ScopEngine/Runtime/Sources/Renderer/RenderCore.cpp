#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vector>
#include <cstdint>

#include <Core/Engine.h>
#include <Platform/Window.h>
#include <Renderer/RenderCore.h>
#include <Renderer/Pipelines/Shader.h>
#include <Renderer/Vulkan/VulkanLoader.h>
#include <Maths/Mat4.h>
#include <Core/Logs.h>

#define KVF_IMPLEMENTATION
#ifdef DEBUG
	#define KVF_ENABLE_VALIDATION_LAYERS
#endif
#define KVF_ASSERT(x) (Scop::Assert(x, "internal kvf assertion " #x))

#if defined(__GNUC__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
		#include <kvf.h>
	#pragma GCC diagnostic pop
#elif defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wmissing-field-initializers"
		#include <kvf.h>
	#pragma clang diagnostic pop
#else
	#include <kvf.h>
#endif

namespace Scop
{
	static std::unique_ptr<VulkanLoader> loader;

	std::optional<std::uint32_t> FindMemoryType(std::uint32_t type_filter, VkMemoryPropertyFlags properties, bool error)
	{
		VkPhysicalDeviceMemoryProperties mem_properties;
		RenderCore::Get().vkGetPhysicalDeviceMemoryProperties(RenderCore::Get().GetPhysicalDevice(), &mem_properties);

		for(std::uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
		{
			if((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}
		if(error)
			FatalError("Vulkan: failed to find suitable memory type");
		return std::nullopt;
	}

	void ErrorCallback(const char* message) noexcept
	{
		Logs::Report(LogType::FatalError, 0, "", "", message);
		std::cout << std::endl;
	}

	void ValidationErrorCallback(const char* message) noexcept
	{
		Logs::Report(LogType::Error, 0, "", "", message);
		std::cout << std::endl;
	}

	void WarningCallback(const char* message) noexcept
	{
		Logs::Report(LogType::Warning, 0, "", "", message);
		std::cout << std::endl;
	}

	RenderCore* RenderCore::s_instance = nullptr;

	RenderCore::RenderCore()
	{
		if(s_instance != nullptr)
			return;

		s_instance = this;

		Window window("", 1, 1, true);
		std::vector<const char*> instance_extensions = window.GetRequiredVulkanInstanceExtentions();

		loader = std::make_unique<VulkanLoader>();

		LoadKVFGlobalVulkanFunctionPointers();

		kvfSetErrorCallback(&ErrorCallback);
		kvfSetWarningCallback(&WarningCallback);
		kvfSetValidationErrorCallback(&ValidationErrorCallback);
		kvfSetValidationWarningCallback(&WarningCallback);

		//kvfAddLayer("VK_LAYER_MESA_overlay");

		m_instance = kvfCreateInstance(instance_extensions.data(), instance_extensions.size());
		Message("Vulkan: instance created");

		loader->LoadInstance(m_instance);
		LoadKVFInstanceVulkanFunctionPointers();

		VkSurfaceKHR surface = window.CreateVulkanSurface(m_instance);

		m_physical_device = kvfPickGoodDefaultPhysicalDevice(m_instance, surface);

		// just for style
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(m_physical_device, &props);
		Message("Vulkan: physical device picked '%'", props.deviceName);

		const char* device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		VkPhysicalDeviceFeatures features{};
		vkGetPhysicalDeviceFeatures(m_physical_device, &features);
		m_device = kvfCreateDevice(m_physical_device, device_extensions, sizeof(device_extensions) / sizeof(device_extensions[0]), &features);
		Message("Vulkan: logical device created");

		loader->LoadDevice(m_device);
		LoadKVFDeviceVulkanFunctionPointers();

		vkDestroySurfaceKHR(m_instance, surface, nullptr);

		m_allocator.AttachToDevice(m_device, m_physical_device);

		ShaderLayout vertex_shader_layout(
			{
				{ 0,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }
					})
				}
			}, { ShaderPushConstantLayout({ 0, sizeof(Mat4f) * 2 }) }
		);
		m_internal_shaders[DEFAULT_VERTEX_SHADER_ID] = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/ForwardVertex.spv", ShaderType::Vertex, std::move(vertex_shader_layout));

		ShaderLayout default_fragment_shader_layout(
			{
				{ 1,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER },
						{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }
					})
				}
			}, {}
		);
		m_internal_shaders[DEFAULT_FRAGMENT_SHADER_ID] = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/ForwardDefaultFragment.spv", ShaderType::Fragment, std::move(default_fragment_shader_layout));

		ShaderLayout basic_fragment_shader_layout(
			{
				{ 1,
					ShaderSetLayout({ 
						{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER },
						{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }
					})
				}
			}, {}
		);
		m_internal_shaders[BASIC_FRAGMENT_SHADER_ID] = LoadShaderFromFile(ScopEngine::Get().GetAssetsPath() / "Shaders/Build/ForwardBasicFragment.spv", ShaderType::Fragment, std::move(basic_fragment_shader_layout));
	}

	#undef SCOP_LOAD_FUNCTION
#define SCOP_LOAD_FUNCTION(fn) pfns.fn = this->fn

	void RenderCore::LoadKVFGlobalVulkanFunctionPointers() const noexcept
	{
		KvfGlobalVulkanFunctions pfns;
		SCOP_LOAD_FUNCTION(vkCreateInstance);
		SCOP_LOAD_FUNCTION(vkEnumerateInstanceExtensionProperties);
		SCOP_LOAD_FUNCTION(vkEnumerateInstanceLayerProperties);
		SCOP_LOAD_FUNCTION(vkGetInstanceProcAddr);
		kvfPassGlobalVulkanFunctionPointers(&pfns);
	}

	void RenderCore::LoadKVFInstanceVulkanFunctionPointers() const noexcept
	{
		KvfInstanceVulkanFunctions pfns;
		SCOP_LOAD_FUNCTION(vkCreateDevice);
		SCOP_LOAD_FUNCTION(vkDestroyInstance);
		SCOP_LOAD_FUNCTION(vkEnumerateDeviceExtensionProperties);
		SCOP_LOAD_FUNCTION(vkEnumeratePhysicalDevices);
		SCOP_LOAD_FUNCTION(vkGetPhysicalDeviceFeatures);
		SCOP_LOAD_FUNCTION(vkGetPhysicalDeviceFormatProperties);
		SCOP_LOAD_FUNCTION(vkGetPhysicalDeviceImageFormatProperties);
		SCOP_LOAD_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
		SCOP_LOAD_FUNCTION(vkGetPhysicalDeviceProperties);
		SCOP_LOAD_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
		SCOP_LOAD_FUNCTION(vkDestroySurfaceKHR);
		SCOP_LOAD_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
		SCOP_LOAD_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
		SCOP_LOAD_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
		SCOP_LOAD_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
		kvfPassInstanceVulkanFunctionPointers(&pfns);
	}

	void RenderCore::LoadKVFDeviceVulkanFunctionPointers() const noexcept
	{
		KvfDeviceVulkanFunctions pfns;
		SCOP_LOAD_FUNCTION(vkAllocateCommandBuffers);
		SCOP_LOAD_FUNCTION(vkAllocateDescriptorSets);
		SCOP_LOAD_FUNCTION(vkBeginCommandBuffer);
		SCOP_LOAD_FUNCTION(vkCmdBeginRenderPass);
		SCOP_LOAD_FUNCTION(vkCmdCopyBuffer);
		SCOP_LOAD_FUNCTION(vkCmdCopyBufferToImage);
		SCOP_LOAD_FUNCTION(vkCmdCopyImage);
		SCOP_LOAD_FUNCTION(vkCmdCopyImageToBuffer);
		SCOP_LOAD_FUNCTION(vkCmdEndRenderPass);
		SCOP_LOAD_FUNCTION(vkCmdPipelineBarrier);
		SCOP_LOAD_FUNCTION(vkCreateBuffer);
		SCOP_LOAD_FUNCTION(vkCreateCommandPool);
		SCOP_LOAD_FUNCTION(vkCreateDescriptorPool);
		SCOP_LOAD_FUNCTION(vkCreateDescriptorSetLayout);
		SCOP_LOAD_FUNCTION(vkCreateFence);
		SCOP_LOAD_FUNCTION(vkCreateFramebuffer);
		SCOP_LOAD_FUNCTION(vkCreateGraphicsPipelines);
		SCOP_LOAD_FUNCTION(vkCreateImage);
		SCOP_LOAD_FUNCTION(vkCreateImageView);
		SCOP_LOAD_FUNCTION(vkCreatePipelineLayout);
		SCOP_LOAD_FUNCTION(vkCreateRenderPass);
		SCOP_LOAD_FUNCTION(vkCreateSampler);
		SCOP_LOAD_FUNCTION(vkCreateSemaphore);
		SCOP_LOAD_FUNCTION(vkCreateShaderModule);
		SCOP_LOAD_FUNCTION(vkDestroyBuffer);
		SCOP_LOAD_FUNCTION(vkDestroyCommandPool);
		SCOP_LOAD_FUNCTION(vkDestroyDescriptorPool);
		SCOP_LOAD_FUNCTION(vkDestroyDescriptorSetLayout);
		SCOP_LOAD_FUNCTION(vkDestroyDevice);
		SCOP_LOAD_FUNCTION(vkDestroyFence);
		SCOP_LOAD_FUNCTION(vkDestroyFramebuffer);
		SCOP_LOAD_FUNCTION(vkDestroyImage);
		SCOP_LOAD_FUNCTION(vkDestroyImageView);
		SCOP_LOAD_FUNCTION(vkDestroyPipeline);
		SCOP_LOAD_FUNCTION(vkDestroyPipelineLayout);
		SCOP_LOAD_FUNCTION(vkDestroyRenderPass);
		SCOP_LOAD_FUNCTION(vkDestroySampler);
		SCOP_LOAD_FUNCTION(vkDestroySemaphore);
		SCOP_LOAD_FUNCTION(vkDestroyShaderModule);
		SCOP_LOAD_FUNCTION(vkDeviceWaitIdle);
		SCOP_LOAD_FUNCTION(vkEndCommandBuffer);
		SCOP_LOAD_FUNCTION(vkFreeCommandBuffers);
		SCOP_LOAD_FUNCTION(vkGetDeviceQueue);
		SCOP_LOAD_FUNCTION(vkGetImageSubresourceLayout);
		SCOP_LOAD_FUNCTION(vkQueueSubmit);
		SCOP_LOAD_FUNCTION(vkResetCommandBuffer);
		SCOP_LOAD_FUNCTION(vkResetDescriptorPool);
		SCOP_LOAD_FUNCTION(vkResetEvent);
		SCOP_LOAD_FUNCTION(vkResetFences);
		SCOP_LOAD_FUNCTION(vkUpdateDescriptorSets);
		SCOP_LOAD_FUNCTION(vkWaitForFences);
		SCOP_LOAD_FUNCTION(vkCreateSwapchainKHR);
		SCOP_LOAD_FUNCTION(vkDestroySwapchainKHR);
		SCOP_LOAD_FUNCTION(vkGetSwapchainImagesKHR);
		SCOP_LOAD_FUNCTION(vkQueuePresentKHR);
		kvfPassDeviceVulkanFunctionPointers(m_physical_device, m_device, &pfns);
	}

#undef SCOP_LOAD_FUNCTION

	RenderCore::~RenderCore()
	{
		if(s_instance == nullptr)
			return;
		WaitDeviceIdle();
		m_allocator.DetachFromDevice();
		for(auto shader: m_internal_shaders)
			shader->Destroy();
		kvfDestroyDevice(m_device);
		Message("Vulkan: logical device destroyed");
		kvfDestroyInstance(m_instance);
		Message("Vulkan: instance destroyed");
		loader.reset();

		s_instance = nullptr;
	}
}
