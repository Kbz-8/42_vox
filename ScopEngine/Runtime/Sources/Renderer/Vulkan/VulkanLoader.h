#ifndef __SCOP_VULKAN_LOADER__
#define __SCOP_VULKAN_LOADER__

#include <vulkan/vulkan_core.h>

namespace Scop
{
	class VulkanLoader
	{
		public:
			VulkanLoader();
			void LoadInstance(VkInstance instance);
			void LoadDevice(VkDevice device);
			~VulkanLoader();

		private:
			void LoadGlobalFunctions(void* context, PFN_vkVoidFunction (*load)(void*, const char*)) noexcept;
			void LoadInstanceFunctions(void* context, PFN_vkVoidFunction (*load)(void*, const char*)) noexcept;
			void LoadDeviceFunctions(void* context, PFN_vkVoidFunction (*load)(void*, const char*)) noexcept;

		private:
			void* p_module = nullptr;
	};
}

#endif
