#include "k423.h"
#include "Shader.h"
#include "Renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <Platform/OpenGL/OpenGLShader.h>
#include <Platform/OpenGL/OpenGLMeshShader.h>

namespace WinterSolstice {
	namespace Bronya {
		Ref<Shader> Shader::Create(const std::string& vs, const std::string& fs)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(vs, fs);
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}
		Ref<Shader> Shader::Create(const std::string& filepath)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(filepath);
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}

		Ref<Shader> Shader::CreateCompute(const std::string& filepath)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLComputeShader>(filepath);
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}

		Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}

		Ref<Shader> Shader::CreateMeshShader(const std::string& name, const std::string& meshShaderSrc)
		{
			switch (Renderer::GetAPI())
			{
			case RendererAPI::API::None:    Kiana_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLMeshShader>(name, meshShaderSrc);
			}

			Kiana_CORE_ASSERT(false, "Unknown RendererAPI!");
			return nullptr;
		}

		void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
		{
			Kiana_CORE_ASSERT(!Exists(name), "Shader already exists!");
			shader->SetName(name);
			m_Shaders[name] = shader;
		}

		void ShaderLibrary::Add(const Ref<Shader>& shader)
		{
			auto& name = shader->GetName();
			Add(name, shader);
		}

		Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
		{
			auto shader = Shader::Create(filepath);
			Add(shader);
			return shader;
		}

		Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
		{
			auto shader = Shader::Create(filepath);
			Add(name, shader);
			return shader;
		}

		Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& vs, const std::string& fs)
		{
			auto shader = Shader::Create(vs, fs);
			Add(name, shader);
			return shader;
		}

		Ref<Shader> ShaderLibrary::Get(const std::string& name)
		{
			Kiana_CORE_ASSERT(Exists(name), "Shader not found!");
			return m_Shaders[name];
		}

		Ref<Shader> ShaderLibrary::GetAs(const std::string& name)
		{
			return Ref<Shader>();
		}

		bool ShaderLibrary::Exists(const std::string& name) const
		{
			return m_Shaders.find(name) != m_Shaders.end();
		}
	}
}