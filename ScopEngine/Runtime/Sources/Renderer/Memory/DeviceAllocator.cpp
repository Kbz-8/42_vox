#include <Renderer/Memory/DeviceAllocator.h>
#include <Renderer/RenderCore.h>
#include <Maths/Constants.h>
#include <Core/Logs.h>

#include <optional>

namespace Scop
{
	#define AlignUp(val, alignment) (val + alignment - 1) & ~(alignment - 1)

	void DeviceAllocator::AttachToDevice(VkDevice device, VkPhysicalDevice physical) noexcept
	{
		m_device = device;
		m_physical = physical;

		RenderCore::Get().vkGetPhysicalDeviceMemoryProperties(physical, &m_mem_props);
	}

	[[nodiscard]] MemoryBlock DeviceAllocator::Allocate(VkDeviceSize size, VkDeviceSize alignment, std::int32_t memory_type_index, bool dedicated_chunk)
	{
		Verify(m_device != VK_NULL_HANDLE, "invalid device");
		Verify(m_physical != VK_NULL_HANDLE, "invalid physical device");
		const std::lock_guard<std::mutex> guard(m_alloc_mutex);
		if(!dedicated_chunk)
		{
			for(auto& chunk : m_chunks)
			{
				if(chunk->GetMemoryTypeIndex() == memory_type_index)
				{
					std::optional<MemoryBlock> block = chunk->Allocate(size, alignment);
					if(block.has_value())
						return *block;
				}
			}
		}
		VkDeviceSize chunk_size = CalcPreferredChunkSize(memory_type_index);
		m_chunks.emplace_back(std::make_unique<MemoryChunk>(m_device, m_physical, chunk_size, memory_type_index, dedicated_chunk));
		std::optional<MemoryBlock> block = m_chunks.back()->Allocate(size, alignment);
		m_allocations_count++;
		if(block.has_value())
			return *block;
		FatalError("Device Allocator: could not allocate a memory block");
		return {}; // to avoid warnings
	}

	void DeviceAllocator::Deallocate(const MemoryBlock& block)
	{
		Verify(m_device != VK_NULL_HANDLE, "invalid device");
		Verify(m_physical != VK_NULL_HANDLE, "invalid physical device");
		const std::lock_guard<std::mutex> guard(m_dealloc_mutex);
		for(auto it = m_chunks.begin(); it != m_chunks.end(); ++it)
		{
			if((*it)->Has(block))
			{
				(*it)->Deallocate(block);
				if((*it)->IsDedicated())
				{
					m_chunks.erase(it);
					m_allocations_count--;
				}
				return;
			}
		}
		Error("Device Allocator: unable to free a block; could not find it's chunk");
	}

	VkDeviceSize DeviceAllocator::CalcPreferredChunkSize(std::uint32_t mem_type_index)
	{
		std::uint32_t heap_index = m_mem_props.memoryTypes[mem_type_index].heapIndex;
		VkDeviceSize heap_size = m_mem_props.memoryHeaps[heap_index].size;
		bool is_small_heap = heap_size <= SMALL_HEAP_MAX_SIZE;
		return AlignUp((is_small_heap ? (heap_size / 8) : DEFAULT_LARGE_HEAP_BLOCK_SIZE), (VkDeviceSize)32);
	}
}
