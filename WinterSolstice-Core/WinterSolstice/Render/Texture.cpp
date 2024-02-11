#include "k423.h"
#include "WinterSolstice/Render/Texture.h"
#include "WinterSolstice/Render/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace WinterSolstice {
	namespace Bronya {
		Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(specification);
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}

		Ref<Texture2D> Texture2D::Create(const std::string& path)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}

		Ref<Texture2D> Texture2D::CreateFrameTexture(uint32_t width, uint32_t height)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFrameBufferTexture>();
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}

		Ref<TextureCube> TextureCube::CreateCubeMap(const std::string& path) {
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTextureCube>(path);
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}
	}
}