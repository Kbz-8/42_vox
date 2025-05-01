#include <Renderer/Vulkan/VulkanLoader.h>
#include <Renderer/RenderCore.h>
#include <Core/Logs.h>

#include <array>
#include <dlfcn.h>

#if defined(__GNUC__)
	#define DISABLE_GCC_PEDANTIC_WARNINGS \
		_Pragma("GCC diagnostic push") \
		_Pragma("GCC diagnostic ignored \"-Wpedantic\"")
	#define RESTORE_GCC_PEDANTIC_WARNINGS \
		_Pragma("GCC diagnostic pop")
#else
	#define DISABLE_GCC_PEDANTIC_WARNINGS
	#define RESTORE_GCC_PEDANTIC_WARNINGS
#endif

namespace Scop
{
	namespace Internal
	{
		static inline PFN_vkVoidFunction vkGetInstanceProcAddrStub(void* context, const char* name)
		{
			PFN_vkVoidFunction function = RenderCore::Get().vkGetInstanceProcAddr(static_cast<VkInstance>(context), name);
			if(!function)
				FatalError("Vulkan Loader: could not load '%'", name);
			//Message("Vulkan Loader: loaded %", name);
			return function;
		}

		static inline PFN_vkVoidFunction vkGetDeviceProcAddrStub(void* context, const char* name)
		{
			PFN_vkVoidFunction function = RenderCore::Get().vkGetDeviceProcAddr(static_cast<VkDevice>(context), name);
			if(!function)
				FatalError("Vulkan Loader: could not load '%'", name);
			//Message("Vulkan Loader: loaded %", name);
			return function;
		}

		static inline void* LoadLib(const char* libname)
		{
			return dlopen(libname, RTLD_NOW | RTLD_LOCAL);
		}

		static inline void* GetSymbol(void* module, const char* name)
		{
			return dlsym(module, name);
		}
	}

	VulkanLoader::VulkanLoader()
	{
		std::array libnames{
			"libvulkan.so.1",
			"libvulkan.so"
		};

		for(auto libname : libnames)
		{
			p_module = Internal::LoadLib(libname);
			if(p_module != nullptr)
			{
				DISABLE_GCC_PEDANTIC_WARNINGS;
				RenderCore::Get().vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(Internal::GetSymbol(p_module, "vkGetInstanceProcAddr"));
				RESTORE_GCC_PEDANTIC_WARNINGS;
				if(RenderCore::Get().vkGetInstanceProcAddr)
				{
					Message("Vulkan Loader: libvulkan loaded using '%'", libname);
					break;
				}
			}
		}
		if(!p_module || !RenderCore::Get().vkGetInstanceProcAddr)
			FatalError("Vulkan Loader: failed to load libvulkan");
		LoadGlobalFunctions(nullptr, Internal::vkGetInstanceProcAddrStub);
	}

	void VulkanLoader::LoadInstance(VkInstance instance)
	{
		LoadInstanceFunctions(instance, Internal::vkGetInstanceProcAddrStub);
	}

	void VulkanLoader::LoadDevice(VkDevice device)
	{
		LoadDeviceFunctions(device, Internal::vkGetDeviceProcAddrStub);
	}

	void VulkanLoader::LoadGlobalFunctions(void* context, PFN_vkVoidFunction (*load)(void*, const char*)) noexcept
	{
		#define SCOP_VULKAN_GLOBAL_FUNCTION(fn) RenderCore::Get().fn = reinterpret_cast<PFN_##fn>(load(context, #fn));
			#include <Renderer/Vulkan/VulkanDefs.h>
		#undef SCOP_VULKAN_GLOBAL_FUNCTION
		Message("Vulkan Loader: global functions loaded");
	}

	void VulkanLoader::LoadInstanceFunctions(void* context, PFN_vkVoidFunction (*load)(void*, const char*)) noexcept
	{
		#define SCOP_VULKAN_INSTANCE_FUNCTION(fn) RenderCore::Get().fn = reinterpret_cast<PFN_##fn>(load(context, #fn));
			#include <Renderer/Vulkan/VulkanDefs.h>
		#undef SCOP_VULKAN_INSTANCE_FUNCTION
		Message("Vulkan Loader: instance functions loaded");
	}

	void VulkanLoader::LoadDeviceFunctions(void* context, PFN_vkVoidFunction (*load)(void*, const char*)) noexcept
	{
		#define SCOP_VULKAN_DEVICE_FUNCTION(fn) RenderCore::Get().fn = reinterpret_cast<PFN_##fn>(load(context, #fn));
			#include <Renderer/Vulkan/VulkanDefs.h>
		#undef SCOP_VULKAN_DEVICE_FUNCTION
		Message("Vulkan Loader: device functions loaded");
	}

	VulkanLoader::~VulkanLoader()
	{
		dlclose(p_module);
		p_module = nullptr;
		Message("Vulkan Loader: libvulkan unloaded");
	}
}
