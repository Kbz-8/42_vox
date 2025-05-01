#ifndef __SCOP_GPU_BUFFER__
#define __SCOP_GPU_BUFFER__

#include <kvf.h>
#include <Renderer/Enums.h>
#include <Core/Logs.h>
#include <Renderer/RenderCore.h>
#include <Utils/Buffer.h>
#include <Renderer/Memory/Block.h>

namespace Scop
{
	class GPUBuffer
	{
		public:
			GPUBuffer() = default;

			void Init(BufferType type, VkDeviceSize size, VkBufferUsageFlags usage, CPUBuffer data);
			void Destroy() noexcept;

			bool CopyFrom(const GPUBuffer& buffer) noexcept;

			void Swap(GPUBuffer& buffer) noexcept;

			[[nodiscard]] inline void* GetMap() const noexcept { return m_memory.map; }
			[[nodiscard]] inline VkBuffer operator()() const noexcept { return m_buffer; }
			[[nodiscard]] inline VkBuffer Get() const noexcept { return m_buffer; }
			[[nodiscard]] inline VkDeviceMemory GetMemory() const noexcept { return m_memory.memory; }
			[[nodiscard]] inline VkDeviceSize GetSize() const noexcept { return m_memory.size; }
			[[nodiscard]] inline VkDeviceSize GetOffset() const noexcept { return 0; }

			[[nodiscard]] inline static std::size_t GetBufferCount() noexcept { return s_buffer_count; }

			[[nodiscard]] inline bool IsInit() const noexcept { return m_buffer != VK_NULL_HANDLE; }

			~GPUBuffer() = default;

		protected:
			void PushToGPU() noexcept;

		protected:
			VkBuffer m_buffer = VK_NULL_HANDLE;
			MemoryBlock m_memory = NULL_MEMORY_BLOCK;

		private:
			void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		private:
			inline static std::size_t s_buffer_count = 0;

			VkBufferUsageFlags m_usage = 0;
			VkMemoryPropertyFlags m_flags = 0;
	};

	class VertexBuffer : public GPUBuffer
	{
		public:
			inline void Init(std::uint32_t size, VkBufferUsageFlags additional_flags = 0) { GPUBuffer::Init(BufferType::LowDynamic, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | additional_flags, {}); }
			void SetData(CPUBuffer data);
			inline void Bind(VkCommandBuffer cmd) const noexcept { VkDeviceSize offset = 0; RenderCore::Get().vkCmdBindVertexBuffers(cmd, 0, 1, &m_buffer, &offset); }
	};

	class IndexBuffer : public GPUBuffer
	{
		public:
			inline void Init(std::uint32_t size, VkBufferUsageFlags additional_flags = 0) { GPUBuffer::Init(BufferType::LowDynamic, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | additional_flags, {}); }
			void SetData(CPUBuffer data);
			inline void Bind(VkCommandBuffer cmd) const noexcept { RenderCore::Get().vkCmdBindIndexBuffer(cmd, m_buffer, 0, VK_INDEX_TYPE_UINT32); }
	};

	class UniformBuffer
	{
		public:
			void Init(std::uint32_t size);
			void SetData(CPUBuffer data, std::size_t frame_index);
			void Destroy() noexcept;

			inline VkDeviceSize GetSize(int i) const noexcept { return m_buffers[i].GetSize(); }
			inline VkDeviceSize GetOffset(int i) const noexcept { return m_buffers[i].GetOffset(); }
			inline VkBuffer GetVk(int i) const noexcept { return m_buffers[i].Get(); }
			inline GPUBuffer& Get(int i) noexcept { return m_buffers[i]; }

		private:
			std::array<GPUBuffer, MAX_FRAMES_IN_FLIGHT> m_buffers;
			std::array<void*, MAX_FRAMES_IN_FLIGHT> m_maps;
	};
}

#endif
