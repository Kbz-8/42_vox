#include <kvf.h>
#include <algorithm>

#include <Core/Logs.h>
#include <Renderer/Image.h>
#include <Renderer/Enums.h>
#include <Renderer/Buffer.h>
#include <Renderer/Descriptor.h>
#include <Renderer/RenderCore.h>

namespace Scop
{
	void TransitionImageToCorrectLayout(Image& image, VkCommandBuffer cmd)
	{
		if(!image.IsInit())
			return;
		if(image.GetType() == ImageType::Color || image.GetType() == ImageType::Cube)
			image.TransitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cmd);
		else if(image.GetType() == ImageType::Depth)
			image.TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, cmd);
		else
			Error("Vulkan: cannot transition descriptor image layout, unkown image type");
	}

	DescriptorSet::DescriptorSet(const ShaderSetLayout& layout, VkDescriptorSetLayout vklayout, ShaderType shader_type)
	: m_set_layout(vklayout)
	{
		for(auto& [binding, type] : layout.binds)
		{
			m_descriptors.emplace_back();
			m_descriptors.back().type = type;
			m_descriptors.back().shader_type = shader_type;
			m_descriptors.back().binding = binding;
		}
		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			m_set[i] = kvfAllocateDescriptorSet(RenderCore::Get().GetDevice(), vklayout);
	}

	DescriptorSet::DescriptorSet(VkDescriptorSetLayout layout, const std::vector<Descriptor>& descriptors)
	: m_set_layout(layout), m_descriptors(descriptors)
	{
		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
			m_set[i] = kvfAllocateDescriptorSet(RenderCore::Get().GetDevice(), layout);
	}

	void DescriptorSet::SetImage(std::size_t i, std::uint32_t binding, class Image& image)
	{
		Verify(m_set[i] != VK_NULL_HANDLE, "invalid descriptor");
		auto it = std::find_if(m_descriptors.begin(), m_descriptors.end(), [=](Descriptor descriptor)
		{
			return binding == descriptor.binding;
		});
		if(it == m_descriptors.end())
		{
			Warning("Vulkan: cannot update descriptor set image; invalid binding");
			return;
		}
		if(it->type != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
		{
			Error("Vulkan: trying to bind an image to the wrong descriptor");
			return;
		}
		it->image_ptr = &image;
	}

	void DescriptorSet::SetStorageBuffer(std::size_t i, std::uint32_t binding, class GPUBuffer& buffer)
	{
		Verify(m_set[i] != VK_NULL_HANDLE, "invalid descriptor");
		auto it = std::find_if(m_descriptors.begin(), m_descriptors.end(), [=](Descriptor descriptor)
		{
			return binding == descriptor.binding;
		});
		if(it == m_descriptors.end())
		{
			Warning("Vulkan: cannot update descriptor set buffer; invalid binding");
			return;
		}
		if(it->type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
		{
			Error("Vulkan: trying to bind a buffer to the wrong descriptor");
			return;
		}
		it->storage_buffer_ptr = &buffer;
	}

	void DescriptorSet::SetUniformBuffer(std::size_t i, std::uint32_t binding, class GPUBuffer& buffer)
	{
		Verify(m_set[i] != VK_NULL_HANDLE, "invalid descriptor");
		auto it = std::find_if(m_descriptors.begin(), m_descriptors.end(), [=](Descriptor descriptor)
		{
			return binding == descriptor.binding;
		});
		if(it == m_descriptors.end())
		{
			Warning("Vulkan: cannot update descriptor set buffer; invalid binding");
			return;
		}
		if(it->type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		{
			Error("Vulkan: trying to bind a buffer to the wrong descriptor");
			return;
		}
		it->uniform_buffer_ptr = &buffer;
	}

	void DescriptorSet::Update(std::size_t i, VkCommandBuffer cmd) noexcept
	{
		Verify(m_set[i] != VK_NULL_HANDLE, "invalid descriptor");

		std::size_t image_count = 0;
		std::size_t buffer_count = 0;

		for(auto& descriptor : m_descriptors)
		{
			if(descriptor.image_ptr)
				image_count++;
			else if(descriptor.uniform_buffer_ptr || descriptor.storage_buffer_ptr)
				buffer_count++;
			else
				FatalError("unknown descriptor data");
		}

		std::vector<VkWriteDescriptorSet> writes(m_descriptors.size());
		std::vector<VkDescriptorBufferInfo> buffer_infos(buffer_count);
		std::vector<VkDescriptorImageInfo> image_infos(image_count);
		std::size_t buffer_index = 0;
		std::size_t image_index = 0;
		std::size_t write_index = 0;

		for(auto& descriptor : m_descriptors)
		{
			if(descriptor.image_ptr)
			{
				TransitionImageToCorrectLayout(*descriptor.image_ptr, cmd);
				VkDescriptorImageInfo info{};
				info.sampler = descriptor.image_ptr->GetSampler();
				info.imageLayout = descriptor.image_ptr->GetLayout();
				info.imageView = descriptor.image_ptr->GetImageView();
				image_infos[image_index] = std::move(info);
				writes[write_index] = kvfWriteImageToDescriptorSet(RenderCore::Get().GetDevice(), m_set[i], &image_infos[image_index], descriptor.binding);
				image_index++;
			}
			else if(descriptor.uniform_buffer_ptr)
			{
				VkDescriptorBufferInfo info{};
				info.buffer = descriptor.uniform_buffer_ptr->Get();
				info.offset = descriptor.uniform_buffer_ptr->GetOffset();
				info.range = VK_WHOLE_SIZE;
				buffer_infos[buffer_index] = std::move(info);
				writes[write_index] = kvfWriteUniformBufferToDescriptorSet(RenderCore::Get().GetDevice(), m_set[i], &buffer_infos[buffer_index], descriptor.binding);
				buffer_index++;
			}
			else if(descriptor.storage_buffer_ptr)
			{
				VkDescriptorBufferInfo info{};
				info.buffer = descriptor.storage_buffer_ptr->Get();
				info.offset = descriptor.storage_buffer_ptr->GetOffset();
				info.range = VK_WHOLE_SIZE;
				buffer_infos[buffer_index] = std::move(info);
				writes[write_index] = kvfWriteStorageBufferToDescriptorSet(RenderCore::Get().GetDevice(), m_set[i], &buffer_infos[buffer_index], descriptor.binding);
				buffer_index++;
			}
			write_index++;
		}
		RenderCore::Get().vkUpdateDescriptorSets(RenderCore::Get().GetDevice(), writes.size(), writes.data(), 0, nullptr);
	}
}
