#ifndef __SCOP_VULKAN_MEMORY_BLOCK__
#define __SCOP_VULKAN_MEMORY_BLOCK__

#include <kvf.h>
#include <algorithm>

namespace Scop
{
	class MemoryBlock
	{
		friend class MemoryChunk;

		public:
			MemoryBlock() = default;

			[[nodiscard]] inline bool operator==(const MemoryBlock& rhs) const noexcept
			{
				return  memory == rhs.memory &&
						offset == rhs.offset &&
						size == rhs.size &&
						free == rhs.free &&
						map == rhs.map;
			}

			inline void Swap(MemoryBlock& rhs) noexcept
			{
				std::swap(memory, rhs.memory);
				std::swap(offset, rhs.offset);
				std::swap(size, rhs.size);
				std::swap(map, rhs.map);
				std::swap(free, rhs.free);
			}

			~MemoryBlock() = default;

		public:
			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkDeviceSize offset = 0;
			VkDeviceSize size = 0;
			void* map = nullptr; // useless if it's a GPU allocation
		
		private:
			bool free = false;
	};

	constexpr MemoryBlock NULL_MEMORY_BLOCK{}; 
}

#endif
