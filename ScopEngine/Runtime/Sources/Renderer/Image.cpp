#include <Renderer/Image.h>
#include <Renderer/RenderCore.h>
#include <Core/Logs.h>

namespace Scop
{
	void Image::Init(ImageType type, std::uint32_t width, std::uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool is_multisampled, std::string_view name)
	{
		m_type = type;
		m_width = width;
		m_height = height;
		m_format = format;
		m_tiling = tiling;
		m_is_multisampled = is_multisampled;

		KvfImageType kvf_type = KVF_IMAGE_OTHER;
		switch(m_type)
		{
			case ImageType::Color: kvf_type = KVF_IMAGE_COLOR; break;
			case ImageType::Depth: kvf_type = KVF_IMAGE_DEPTH; break;
			case ImageType::Cube:  kvf_type = KVF_IMAGE_CUBE;  break;

			default: break;
		}

		if(m_is_multisampled)
		{
			VkImageCreateInfo image_info{};
			image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_info.imageType = VK_IMAGE_TYPE_2D;
			image_info.extent.width = width;
			image_info.extent.height = height;
			image_info.extent.depth = 1;
			image_info.mipLevels = 1;
			image_info.arrayLayers = (m_type == ImageType::Cube ? 6 : 1);
			image_info.format = format;
			image_info.tiling = tiling;
			image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			image_info.usage = usage;
			image_info.samples = VK_SAMPLE_COUNT_4_BIT;
			image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			kvfCheckVk(RenderCore::Get().vkCreateImage(RenderCore::Get().GetDevice(), &image_info, nullptr, &m_image));
		}
		else
			m_image = kvfCreateImage(RenderCore::Get().GetDevice(), width, height, format, tiling, usage, kvf_type);

		VkMemoryRequirements mem_requirements;
		RenderCore::Get().vkGetImageMemoryRequirements(RenderCore::Get().GetDevice(), m_image, &mem_requirements);

		m_memory = RenderCore::Get().GetAllocator().Allocate(mem_requirements.size, mem_requirements.alignment, *FindMemoryType(mem_requirements.memoryTypeBits, properties), true);
		RenderCore::Get().vkBindImageMemory(RenderCore::Get().GetDevice(), m_image, m_memory.memory, 0);

		#ifdef SCOP_HAS_DEBUG_UTILS_FUNCTIONS
			VkDebugUtilsObjectNameInfoEXT name_info{};
			name_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			name_info.objectType = VK_OBJECT_TYPE_IMAGE;
			name_info.objectHandle = reinterpret_cast<std::uint64_t>(m_image);
			name_info.pObjectName = name.data();
			RenderCore::Get().vkSetDebugUtilsObjectNameEXT(RenderCore::Get().GetDevice(), &name_info);
			Message("Vulkan: % image created", name);
		#else
			Message("Vulkan: image created");
		#endif

		s_image_count++;
	}

	void Image::CreateImageView(VkImageViewType type, VkImageAspectFlags aspect_flags, int layer_count) noexcept
	{
		m_image_view = kvfCreateImageView(RenderCore::Get().GetDevice(), m_image, m_format, type, aspect_flags, layer_count);
	}

	void Image::CreateSampler() noexcept
	{
		m_sampler = kvfCreateSampler(RenderCore::Get().GetDevice(), VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_MIPMAP_MODE_NEAREST);
	}

	void Image::TransitionLayout(VkImageLayout new_layout, VkCommandBuffer cmd)
	{
		if(new_layout == m_layout)
			return;
		bool is_single_time_cmd_buffer = (cmd == VK_NULL_HANDLE);
		KvfImageType kvf_type = KVF_IMAGE_OTHER;
		switch(m_type)
		{
			case ImageType::Color: kvf_type = KVF_IMAGE_COLOR; break;
			case ImageType::Depth: kvf_type = KVF_IMAGE_DEPTH; break;
			case ImageType::Cube: kvf_type = KVF_IMAGE_CUBE; break;
			default: break;
		}
		if(is_single_time_cmd_buffer)
			cmd = kvfCreateCommandBuffer(RenderCore::Get().GetDevice());
		kvfTransitionImageLayout(RenderCore::Get().GetDevice(), m_image, kvf_type, cmd, m_format, m_layout, new_layout, is_single_time_cmd_buffer);
		if(is_single_time_cmd_buffer)
			kvfDestroyCommandBuffer(RenderCore::Get().GetDevice(), cmd);
		m_layout = new_layout;
	}

	void Image::Clear(VkCommandBuffer cmd, Vec4f color)
	{
		VkImageSubresourceRange subresource_range{};
		subresource_range.baseMipLevel = 0;
		subresource_range.layerCount = (m_type == ImageType::Cube ? 6 : 1);
		subresource_range.levelCount = 1;
		subresource_range.baseArrayLayer = 0;

		if(m_type == ImageType::Color || m_type == ImageType::Cube)
		{
			VkImageLayout old_layout = m_layout;
			subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cmd);
			VkClearColorValue clear_color = VkClearColorValue({ { color.x, color.y, color.z, color.w } });
			RenderCore::Get().vkCmdClearColorImage(cmd, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_color, 1, &subresource_range);
			TransitionLayout(old_layout, cmd);
		}
		else if(m_type == ImageType::Depth)
		{
			VkClearDepthStencilValue clear_depth_stencil = { 1.0f, 1 };
			subresource_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cmd);
			RenderCore::Get().vkCmdClearDepthStencilImage(cmd, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_depth_stencil, 1, &subresource_range);
			TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, cmd);
		}
	}

	void Image::DestroySampler() noexcept
	{
		if(m_sampler != VK_NULL_HANDLE)
			kvfDestroySampler(RenderCore::Get().GetDevice(), m_sampler);
		m_sampler = VK_NULL_HANDLE;
	}

	void Image::DestroyImageView() noexcept
	{
		if(m_image_view != VK_NULL_HANDLE)
			kvfDestroyImageView(RenderCore::Get().GetDevice(), m_image_view);
		m_image_view = VK_NULL_HANDLE;
	}

	void Image::Destroy() noexcept
	{
		if(m_image == VK_NULL_HANDLE && m_image_view == VK_NULL_HANDLE && m_sampler == VK_NULL_HANDLE)
			return;
		RenderCore::Get().WaitDeviceIdle();
		DestroySampler();
		DestroyImageView();

		if(m_image != VK_NULL_HANDLE)
		{
			RenderCore::Get().GetAllocator().Deallocate(m_memory);
			m_memory = NULL_MEMORY_BLOCK;
			kvfDestroyImage(RenderCore::Get().GetDevice(), m_image);
		}
		Message("Vulkan: image destroyed");
		m_image = VK_NULL_HANDLE;
		m_memory = NULL_MEMORY_BLOCK;
		m_image = VK_NULL_HANDLE;
		m_image_view = VK_NULL_HANDLE;
		m_sampler = VK_NULL_HANDLE;
		m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
		m_width = 0;
		m_height = 0;
		m_is_multisampled = false;
		s_image_count--;
	}

	void CubeTexture::Init(CPUBuffer pixels, std::uint32_t width, std::uint32_t height, VkFormat format, std::string_view name)
	{
		if(!pixels)
			FatalError("Vulkan: a cubemap cannot be created without pixels data");

		std::array<std::vector<std::uint8_t>, 6> texture_data;
		std::uint32_t face_width = width / 4;
		std::uint32_t face_height = height / 3;

		std::size_t size = 0;

		for(std::uint32_t cy = 0, face = 0; cy < 3; cy++)
		{
			for(std::uint32_t cx = 0; cx < 4; cx++)
			{
				if(cx == 0 || cx == 2 || cx == 3)
				{
					if(cy != 1)
						continue;
				}

				texture_data[face] = std::vector<std::uint8_t>(face_width * face_height * sizeof(std::uint32_t));

				size += sizeof(std::uint32_t) * width * height;

				for(std::uint32_t y = 0; y < face_height; y++)
				{
					std::uint32_t offset = y;
					std::uint32_t yp = cy * face_height + offset;
					for(std::uint32_t x = 0; x < face_width; x++)
					{
						offset = x;
						std::uint32_t xp = cx * face_width + offset;
						texture_data[face][(x + y * face_width) * sizeof(std::uint32_t) + 0] = pixels.GetData()[(xp + yp * width) * sizeof(std::uint32_t) + 0];
						texture_data[face][(x + y * face_width) * sizeof(std::uint32_t) + 1] = pixels.GetData()[(xp + yp * width) * sizeof(std::uint32_t) + 1];
						texture_data[face][(x + y * face_width) * sizeof(std::uint32_t) + 2] = pixels.GetData()[(xp + yp * width) * sizeof(std::uint32_t) + 2];
						texture_data[face][(x + y * face_width) * sizeof(std::uint32_t) + 3] = pixels.GetData()[(xp + yp * width) * sizeof(std::uint32_t) + 3];
					}
				}
				face++;
			}
		}

		CPUBuffer complete_data(size);
		std::uint32_t pointer_offset = 0;

		const std::uint32_t face_order[6] = { 3, 1, 0, 5, 2, 4 };

		for(std::uint32_t face : face_order)
		{
			std::size_t current_size = face_width * face_height * sizeof(std::uint32_t);
			std::memcpy(complete_data.GetData() + pointer_offset, texture_data[face].data(), current_size);
			pointer_offset += current_size;
		}

		Image::Init(ImageType::Cube, face_width, face_height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false, std::move(name));
		Image::CreateImageView(VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 6);
		Image::CreateSampler();

		GPUBuffer staging_buffer;
		staging_buffer.Init(BufferType::Staging, complete_data.GetSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, complete_data);
		std::vector<VkBufferImageCopy> buffer_copy_regions;
		std::uint32_t offset = 0;

		for(std::uint32_t face = 0; face < 6; face++)
		{
			VkBufferImageCopy buffer_copy_region{};
			buffer_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			buffer_copy_region.imageSubresource.mipLevel = 0;
			buffer_copy_region.imageSubresource.baseArrayLayer = face;
			buffer_copy_region.imageSubresource.layerCount = 1;
			buffer_copy_region.imageExtent.width = face_width;
			buffer_copy_region.imageExtent.height = face_height;
			buffer_copy_region.imageExtent.depth = 1;
			buffer_copy_region.bufferOffset = offset;
			buffer_copy_regions.push_back(buffer_copy_region);

			offset += face_width * face_height * kvfFormatSize(format);
		}

		VkImageSubresourceRange subresource_range{};
		subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresource_range.baseMipLevel = 0;
		subresource_range.levelCount = 1;
		subresource_range.layerCount = 6;

		auto device = RenderCore::Get().GetDevice();

		VkCommandBuffer cmd = kvfCreateCommandBuffer(device);
		kvfBeginCommandBuffer(cmd, 0);
		TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cmd);
		RenderCore::Get().vkCmdCopyBufferToImage(cmd, staging_buffer.Get(), Image::Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, buffer_copy_regions.size(), buffer_copy_regions.data());
		TransitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cmd);
		kvfEndCommandBuffer(cmd);

		VkFence fence = kvfCreateFence(device);
		kvfSubmitSingleTimeCommandBuffer(device, cmd, KVF_GRAPHICS_QUEUE, fence);
		kvfWaitForFence(device, fence);
		kvfDestroyFence(device, fence);

		staging_buffer.Destroy();
	}
}
