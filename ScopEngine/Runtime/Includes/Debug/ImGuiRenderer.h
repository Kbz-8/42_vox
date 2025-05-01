#ifndef __SCOP_IMGUI_RENDERER__
#define __SCOP_IMGUI_RENDERER__

#include <vector>

#include <kvf.h>
#include <Renderer/RenderCore.h>
#include <Renderer/Renderer.h>
#include <Utils/NonOwningPtr.h>

namespace Scop
{
	class ImGuiRenderer
	{
		public:
			ImGuiRenderer(NonOwningPtr<Renderer> renderer);

			void Init(class Inputs& inputs);
			void Destroy();

			bool BeginFrame();
			void DisplayRenderStatistics();
			void EndFrame();

			~ImGuiRenderer() = default;

		private:
			void SetTheme();
			void CreateFramebuffers();

		private:
			std::vector<VkFramebuffer> m_framebuffers;
			VkRenderPass m_renderpass = VK_NULL_HANDLE;
			VkDescriptorPool m_pool = VK_NULL_HANDLE;
			NonOwningPtr<Renderer> p_renderer;
	};
}

#endif
