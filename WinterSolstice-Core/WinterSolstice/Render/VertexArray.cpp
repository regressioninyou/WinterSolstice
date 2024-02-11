#include "k423.h"
#include "WinterSolstice/Render/VertexArray.h"

#include "WinterSolstice/Render/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace WinterSolstice {
	namespace Bronya {

		Ref<VertexArray> VertexArray::Create()
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexArray>();
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}
	}
}