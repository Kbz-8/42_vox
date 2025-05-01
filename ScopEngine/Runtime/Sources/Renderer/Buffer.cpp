#include <Renderer/RenderCore.h>
#include <Core/Logs.h>
#include <Renderer/Buffer.h>

namespace Scop
{
	void GPUBuffer::Init(BufferType type, VkDeviceSize size, VkBufferUsageFlags usage, CPUBuffer data)
	{
		if(type == BufferType::Constant)
		{
			if(data.Empty())
			{
				Warning("Vulkan: trying to create constant buffer without data (constant buffers cannot be modified after creation)");
				return;
			}
			m_usage = usage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			m_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}
		else if(type == BufferType::HighDynamic)
		{
			m_usage = usage;
			m_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}
		else // LowDynamic or Staging
		{
			m_usage = usage | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			m_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		}

		if(type == BufferType::Staging && data.Empty())
			Warning("Vulkan: trying to create staging buffer without data (wtf?)");

		CreateBuffer(size, m_usage, m_flags);

		if(!data.Empty())
		{
			if(m_memory.map != nullptr)
				std::memcpy(m_memory.map, data.GetData(), data.GetSize());
		}
		if(type == BufferType::Constant || type == BufferType::LowDynamic)
			PushToGPU();
	}

	void GPUBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
	{
		auto device = RenderCore::Get().GetDevice();
		m_buffer = kvfCreateBuffer(device, usage, size);

		VkMemoryRequirements mem_requirements;
		RenderCore::Get().vkGetBufferMemoryRequirements(device, m_buffer, &mem_requirements);

		m_memory = RenderCore::Get().GetAllocator().Allocate(size, mem_requirements.alignment, *FindMemoryType(mem_requirements.memoryTypeBits, properties));
		//m_memory = RenderCore::Get().GetAllocator().Allocate(mem_requirements.size, mem_requirements.alignment, *FindMemoryType(mem_requirements.memoryTypeBits, properties));
		RenderCore::Get().vkBindBufferMemory(device, m_buffer, m_memory.memory, m_memory.offset);
		Message("Vulkan: created buffer");
		s_buffer_count++;
	}

	bool GPUBuffer::CopyFrom(const GPUBuffer& buffer) noexcept
	{
		if(!(m_usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT))
		{
			Error("Vulkan: buffer cannot be the destination of a copy because it does not have the correct usage flag");
			return false;
		}
		if(!(buffer.m_usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT))
		{
			Error("Vulkan: buffer cannot be the source of a copy because it does not have the correct usage flag");
			return false;
		}

		VkCommandBuffer cmd = kvfCreateCommandBuffer(RenderCore::Get().GetDevice());
		kvfBeginCommandBuffer(cmd, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		kvfCopyBufferToBuffer(cmd, m_buffer, buffer.Get(), buffer.GetSize());
		kvfEndCommandBuffer(cmd);
		VkFence fence = kvfCreateFence(RenderCore::Get().GetDevice());
		kvfSubmitSingleTimeCommandBuffer(RenderCore::Get().GetDevice(), cmd, KVF_GRAPHICS_QUEUE, fence);
		kvfWaitForFence(RenderCore::Get().GetDevice(), fence);	
		kvfDestroyFence(RenderCore::Get().GetDevice(), fence);
		return true;
	}

	void GPUBuffer::PushToGPU() noexcept
	{
		GPUBuffer new_buffer;
		new_buffer.m_usage = (this->m_usage & 0xFFFFFFFC) | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		new_buffer.m_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		new_buffer.CreateBuffer(m_memory.size, new_buffer.m_usage, new_buffer.m_flags);

		if(new_buffer.CopyFrom(*this))
			Swap(new_buffer);
		new_buffer.Destroy();
		Message("Vulkan: pushed buffer to GPU memory");
	}

	void GPUBuffer::Destroy() noexcept
	{
		if(m_buffer == VK_NULL_HANDLE)
			return;
		RenderCore::Get().vkDestroyBuffer(RenderCore::Get().GetDevice(), m_buffer, nullptr);
		RenderCore::Get().GetAllocator().Deallocate(m_memory);
		m_buffer = VK_NULL_HANDLE;
		m_memory = NULL_MEMORY_BLOCK;
		Message("Vulkan: destroyed buffer");
		s_buffer_count--;
	}

	void GPUBuffer::Swap(GPUBuffer& buffer) noexcept
	{
		std::swap(m_buffer, buffer.m_buffer);
		m_memory.Swap(buffer.m_memory);
		std::swap(m_usage, buffer.m_usage);
		std::swap(m_flags, buffer.m_flags);
	}

	void VertexBuffer::SetData(CPUBuffer data)
	{
		if(data.GetSize() > m_memory.size)
		{
			Error("Vulkan: trying to store to much data in a vertex buffer (% bytes in % bytes)", data.GetSize(), m_memory.size);
			return;
		}
		if(data.Empty())
		{
			Warning("Vulkan: cannot set empty data in a vertex buffer");
			return;
		}
		GPUBuffer staging;
		staging.Init(BufferType::Staging, data.GetSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, data);
		CopyFrom(staging);
		staging.Destroy();
	}

	void IndexBuffer::SetData(CPUBuffer data)
	{
		if(data.GetSize() > m_memory.size)
		{
			Error("Vulkan: trying to store to much data in an index buffer (% bytes in % bytes)", data.GetSize(), m_memory.size);
			return;
		}
		if(data.Empty())
		{
			Warning("Vulkan: cannot set empty data in an index buffer");
			return;
		}
		GPUBuffer staging;
		staging.Init(BufferType::Staging, data.GetSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, data);
		CopyFrom(staging);
		staging.Destroy();
	}

	void UniformBuffer::Init(std::uint32_t size)
	{
		for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_buffers[i].Init(BufferType::HighDynamic, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, {});
			m_maps[i] = m_buffers[i].GetMap();
			if(m_maps[i] == nullptr)
				FatalError("Vulkan: unable to map a uniform buffer");
		}
	}

	void UniformBuffer::SetData(CPUBuffer data, std::size_t frame_index)
	{
		if(data.GetSize() > m_buffers[frame_index].GetSize())
		{
			Error("Vulkan: invalid data size to update to a uniform buffer, % != %", data.GetSize(), m_buffers[frame_index].GetSize());
			return;
		}
		if(m_maps[frame_index] != nullptr)
			std::memcpy(m_maps[frame_index], data.GetData(), data.GetSize());
	}

	void UniformBuffer::Destroy() noexcept
	{
		for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			m_buffers[i].Destroy();
	}
}
