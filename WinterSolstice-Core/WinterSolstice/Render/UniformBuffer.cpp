#include "k423.h"
#include "UniformBuffer.h"

#include "WinterSolstice/Render/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace WinterSolstice {
	namespace Bronya {

		Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}
	}
}