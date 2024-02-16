#include "k423.h"
#include "Framebuffer.h"
#include "WinterSolstice/Render/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace WinterSolstice {
	namespace Bronya {
		Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}

		Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec, uint32_t level)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<MultiLevelFramebuffer>(spec, level);
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}

	}
}