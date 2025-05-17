#ifndef __SCOP_VULKAN_MEMORY_DEVICE_ALLOCATOR__
#define __SCOP_VULKAN_MEMORY_DEVICE_ALLOCATOR__

#include <mutex>
#include <vector>
#include <memory>
#include <cstdint>

#include <Renderer/Memory/Block.h>
#include <Renderer/Memory/Chunk.h>

namespace Scop
{
	class DeviceAllocator
	{
		public:
			DeviceAllocator() = default;

			inline void AttachToDevice(VkDevice device, VkPhysicalDevice physical) noexcept { m_device = device; m_physical = physical; }
			inline void DetachFromDevice() noexcept { m_chunks.clear(); m_device = VK_NULL_HANDLE; m_physical = VK_NULL_HANDLE; }
			[[nodiscard]] inline std::size_t GetAllocationsCount() const noexcept { return m_allocations_count; }

			[[nodiscard]] MemoryBlock Allocate(VkDeviceSize size, VkDeviceSize alignment, std::int32_t memory_type_index, bool dedicated_chunk = false);
			void Deallocate(const MemoryBlock& block);

			~DeviceAllocator() = default;

		private:
			std::vector<std::unique_ptr<MemoryChunk>> m_chunks;
			VkDevice m_device = VK_NULL_HANDLE;
			VkPhysicalDevice m_physical = VK_NULL_HANDLE;
			std::size_t m_allocations_count = 0;
			std::mutex m_alloc_mutex;
			std::mutex m_dealloc_mutex;
	};
}

#endif
