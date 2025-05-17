#ifndef __SCOP_RENDER_CORE__
#define __SCOP_RENDER_CORE__

#include <array>
#include <memory>
#include <cstdint>
#include <optional>

#include <kvf.h>

#include <Renderer/Memory/DeviceAllocator.h>

namespace Scop
{
	constexpr const int MAX_FRAMES_IN_FLIGHT = 2;

	constexpr const int DEFAULT_VERTEX_SHADER_ID = 0;
	constexpr const int DEFAULT_FRAGMENT_SHADER_ID = 1;
	constexpr const int BASIC_FRAGMENT_SHADER_ID = 2;

	std::optional<std::uint32_t> FindMemoryType(std::uint32_t type_filter, VkMemoryPropertyFlags properties, bool error = true);

	#if defined(DEBUG) && defined(VK_EXT_debug_utils)
		#define SCOP_HAS_DEBUG_UTILS_FUNCTIONS
	#endif

	class RenderCore
	{
		public:
			RenderCore();

			[[nodiscard]] inline VkInstance GetInstance() const noexcept { return m_instance; }
			[[nodiscard]] inline VkInstance& GetInstanceRef() noexcept { return m_instance; }
			[[nodiscard]] inline VkDevice GetDevice() const noexcept { return m_device; }
			[[nodiscard]] inline VkPhysicalDevice GetPhysicalDevice() const noexcept { return m_physical_device; }
			[[nodiscard]] inline DeviceAllocator& GetAllocator() noexcept { return m_allocator; }
			[[nodiscard]] inline bool StackSubmits() const noexcept { return m_stack_submits; }

			[[nodiscard]] inline std::shared_ptr<class Shader> GetDefaultVertexShader() const { return m_internal_shaders[DEFAULT_VERTEX_SHADER_ID]; }
			[[nodiscard]] inline std::shared_ptr<class Shader> GetBasicFragmentShader() const { return m_internal_shaders[BASIC_FRAGMENT_SHADER_ID]; }
			[[nodiscard]] inline std::shared_ptr<class Shader> GetDefaultFragmentShader() const { return m_internal_shaders[DEFAULT_FRAGMENT_SHADER_ID]; }

			inline void WaitDeviceIdle() const noexcept { vkDeviceWaitIdle(m_device); }
			inline void WaitQueueIdle(KvfQueueType queue) const noexcept { vkQueueWaitIdle(kvfGetDeviceQueue(m_device, queue)); }

			inline static bool IsInit() noexcept { return s_instance != nullptr; }
			inline static RenderCore& Get() noexcept { return *s_instance; }

			inline void ShouldStackSubmits(bool should) noexcept { m_stack_submits = should; }

			#define SCOP_VULKAN_GLOBAL_FUNCTION(fn) PFN_##fn fn = nullptr;
			#define SCOP_VULKAN_INSTANCE_FUNCTION(fn) PFN_##fn fn = nullptr;
			#define SCOP_VULKAN_DEVICE_FUNCTION(fn) PFN_##fn fn = nullptr;
				#include <Renderer/Vulkan/VulkanDefs.h>
			#undef SCOP_VULKAN_GLOBAL_FUNCTION
			#undef SCOP_VULKAN_INSTANCE_FUNCTION
			#undef SCOP_VULKAN_DEVICE_FUNCTION

			~RenderCore();

		private:
			void LoadKVFGlobalVulkanFunctionPointers() const noexcept;
			void LoadKVFInstanceVulkanFunctionPointers() const noexcept;
			void LoadKVFDeviceVulkanFunctionPointers() const noexcept;

		private:
			static RenderCore* s_instance;

			std::array<std::shared_ptr<class Shader>, 3> m_internal_shaders;
			DeviceAllocator m_allocator;
			VkInstance m_instance = VK_NULL_HANDLE;
			VkDevice m_device = VK_NULL_HANDLE;
			VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
			bool m_stack_submits = false;
	};
}

#endif
