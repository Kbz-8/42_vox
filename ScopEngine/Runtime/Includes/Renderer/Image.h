#ifndef __SCOP_IMAGE__
#define __SCOP_IMAGE__

#include <cstdint>
#include <vector>
#include <kvf.h>

#include <Maths/Vec4.h>
#include <Renderer/RenderCore.h>
#include <Renderer/Buffer.h>
#include <Utils/Buffer.h>
#include <Renderer/Enums.h>
#include <Renderer/Memory/Block.h>

namespace Scop
{
	class Image
	{
		public:
			Image() = default;

			inline void Init(VkImage image, VkFormat format, std::uint32_t width, std::uint32_t height, VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED, std::string_view name = {}) noexcept
			{
				m_image = image;
				m_format = format;
				m_width = width;
				m_height = height;
				m_layout = layout;
				#ifdef SCOP_HAS_DEBUG_UTILS_FUNCTIONS
					VkDebugUtilsObjectNameInfoEXT name_info{};
					name_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
					name_info.objectType = VK_OBJECT_TYPE_IMAGE;
					name_info.objectHandle = reinterpret_cast<std::uint64_t>(m_image);
					name_info.pObjectName = name.data();
					RenderCore::Get().vkSetDebugUtilsObjectNameEXT(RenderCore::Get().GetDevice(), &name_info);
				#endif
			}

			void Init(ImageType type, std::uint32_t width, std::uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool is_multisampled = false, std::string_view name = {}, bool dedicated_alloc = false);
			void CreateImageView(VkImageViewType type, VkImageAspectFlags aspectFlags, int layer_count = 1) noexcept;
			void CreateSampler() noexcept;
			void TransitionLayout(VkImageLayout new_layout, VkCommandBuffer cmd = VK_NULL_HANDLE);
			void Clear(VkCommandBuffer cmd, Vec4f color);

			void DestroySampler() noexcept;
			void DestroyImageView() noexcept;
			virtual void Destroy() noexcept;

			[[nodiscard]] inline VkImage Get() const noexcept { return m_image; }
			[[nodiscard]] inline VkImage operator()() const noexcept { return m_image; }
			[[nodiscard]] inline VkDeviceMemory GetDeviceMemory() const noexcept { return m_memory.memory; }
			[[nodiscard]] inline VkImageView GetImageView() const noexcept { return m_image_view; }
			[[nodiscard]] inline VkFormat GetFormat() const noexcept { return m_format; }
			[[nodiscard]] inline VkImageTiling GetTiling() const noexcept { return m_tiling; }
			[[nodiscard]] inline VkImageLayout GetLayout() const noexcept { return m_layout; }
			[[nodiscard]] inline VkSampler GetSampler() const noexcept { return m_sampler; }
			[[nodiscard]] inline std::uint32_t GetWidth() const noexcept { return m_width; }
			[[nodiscard]] inline std::uint32_t GetHeight() const noexcept { return m_height; }
			[[nodiscard]] inline bool IsInit() const noexcept { return m_image != VK_NULL_HANDLE; }
			[[nodiscard]] inline ImageType GetType() const noexcept { return m_type; }

			[[nodiscard]] inline static std::size_t GetImageCount() noexcept { return s_image_count; }

			virtual ~Image() = default;

		private:
			inline static std::size_t s_image_count = 0;

			MemoryBlock m_memory = NULL_MEMORY_BLOCK;
			VkImage m_image = VK_NULL_HANDLE;
			VkImageView m_image_view = VK_NULL_HANDLE;
			VkSampler m_sampler = VK_NULL_HANDLE;
			VkFormat m_format;
			VkImageTiling m_tiling;
			VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
			ImageType m_type;
			std::uint32_t m_width = 0;
			std::uint32_t m_height = 0;
			bool m_is_multisampled = false;
	};

	class DepthImage : public Image
	{
		public:
			DepthImage() = default;
			inline void Init(std::uint32_t width, std::uint32_t height, bool is_multisampled = false, std::string_view name = {})
			{
				std::vector<VkFormat> candidates = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
				VkFormat format = kvfFindSupportFormatInCandidates(RenderCore::Get().GetDevice(), candidates.data(), candidates.size(), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
				Image::Init(ImageType::Depth, width, height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, is_multisampled, std::move(name)); 
				Image::TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
				Image::CreateImageView(VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT);
			}
			~DepthImage() = default;
	};

	class Texture : public Image
	{
		public:
			Texture() = default;
			Texture(CPUBuffer pixels, std::uint32_t width, std::uint32_t height, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, bool is_multisampled = false, std::string_view name = {}, bool dedicated_alloc = false)
			{
				Init(std::move(pixels), width, height, format, is_multisampled, std::move(name), dedicated_alloc);
			}
			inline void Init(CPUBuffer pixels, std::uint32_t width, std::uint32_t height, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, bool is_multisampled = false, std::string_view name = {}, bool dedicated_alloc = false)
			{
				Image::Init(ImageType::Color, width, height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, is_multisampled, std::move(name), dedicated_alloc);
				Image::CreateImageView(VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT);
				Image::CreateSampler();
				if(pixels)
				{
					TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
					GPUBuffer staging_buffer;
					std::size_t size = width * height * kvfFormatSize(format);
					staging_buffer.Init(BufferType::Staging, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, pixels);
					VkCommandBuffer cmd = kvfCreateCommandBuffer(RenderCore::Get().GetDevice());
					kvfBeginCommandBuffer(cmd, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
					kvfCopyBufferToImage(cmd, Image::Get(), staging_buffer.Get(), staging_buffer.GetOffset(), VK_IMAGE_ASPECT_COLOR_BIT, { width, height, 1 });
					RenderCore::Get().vkEndCommandBuffer(cmd);
					VkFence fence = kvfCreateFence(RenderCore::Get().GetDevice());
					kvfSubmitSingleTimeCommandBuffer(RenderCore::Get().GetDevice(), cmd, KVF_GRAPHICS_QUEUE, fence);
					kvfDestroyFence(RenderCore::Get().GetDevice(), fence);
					staging_buffer.Destroy();
				}
				if(!pixels)
					TransitionLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
				else
					TransitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			}
			~Texture() override { Destroy(); }
	};

	class CubeTexture : public Image
	{
		public:
			CubeTexture() = default;
			CubeTexture(CPUBuffer pixels, std::uint32_t width, std::uint32_t height, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, std::string_view name = {})
			{
				Init(std::move(pixels), width, height, format, std::move(name));
			}
			void Init(CPUBuffer pixels, std::uint32_t width, std::uint32_t height, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, std::string_view name = {});
			~CubeTexture() override { Destroy(); }
	};
}

#endif
