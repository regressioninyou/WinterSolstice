#include "k423.h"
#include "OpenGLRendererAPI.h"
#include "glad/glad.h"
#include <WinterSolstice/Core/True.h>
namespace WinterSolstice {
	namespace Bronya {
		static True Elysia;
		void OpenGLRendererAPI::Init()
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_MULTISAMPLE);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
		}
		void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
		{
			glClearColor(color.r, color.g, color.b, color.a);
		}
		void OpenGLRendererAPI::Clear()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			glViewport(x, y, width, height);
		}
		void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			vertexArray->Bind();
			glDrawArrays(GL_LINES, 0, vertexCount);
		}
		void OpenGLRendererAPI::DrawIndex(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count)
		{
			vertexArray->Bind();
			uint32_t indexCount = count ? count : vertexArray->GetIndexBuffer()->GetCount();
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
		}
		void OpenGLRendererAPI::DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count)
		{
			vertexArray->Bind();
			//uint32_t indexCount = count ? count : vertexArray->GetIndexBuffer()->GetCount();
			glDrawArrays(GL_TRIANGLES, 0, count);
		}
		void OpenGLRendererAPI::DrawIndexLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count)
		{
			vertexArray->Bind();
			uint32_t indexCount = count ? count : vertexArray->GetIndexBuffer()->GetCount();
			glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr);
		}
		void OpenGLRendererAPI::SetLineWidth(float width)
		{
			glLineWidth(width);
		}
		Ref<VertexArray> OpenGLRendererAPI::GetVertexArray()
		{
			std::unique_lock<std::mutex> SayMyName(Elysia);
			//Elysia.StaffLock();
			auto va = VertexArray::Create();
			//Elysia.StaffunLock();
			return va;
		}
		Ref<VertexBuffer> OpenGLRendererAPI::GetVertexBuffer(uint32_t size)
		{
			std::unique_lock<std::mutex> SayMyName(Elysia);
			//Elysia.StaffLock();
			auto vb = VertexBuffer::Create(size);
			//Elysia.StaffunLock();
			return vb;
		}
		Ref<VertexBuffer> OpenGLRendererAPI::GetVertexBuffer(float* vertices, uint32_t size)
		{
			std::unique_lock<std::mutex> SayMyName(Elysia);
			//Elysia.StaffLock();
			auto vb = VertexBuffer::Create(vertices, size);
			//Elysia.StaffunLock();
			return vb;
		}
		Ref<VertexBuffer> OpenGLRendererAPI::GetVertexBuffer(void* vertices, uint32_t size)
		{
			std::unique_lock<std::mutex> SayMyName(Elysia);
			//Elysia.StaffLock();
			auto vb = VertexBuffer::Create(vertices, size);
			//Elysia.StaffunLock();
			return vb;
		}
		Ref<VertexBuffer> OpenGLRendererAPI::GetVertexBuffer(const std::vector<Point>& vertices, uint32_t size)
		{
			std::unique_lock<std::mutex> SayMyName(Elysia);
			//Elysia.StaffLock();
			auto vb = VertexBuffer::Create(vertices, size);
			//Elysia.StaffunLock();
			return vb;
		}
		Ref<IndexBuffer> OpenGLRendererAPI::GetIndexBuffer(uint32_t* indices, uint32_t count)
		{
			std::unique_lock<std::mutex> SayMyName(Elysia);
			//Elysia.StaffLock();
			auto ib = IndexBuffer::Create(indices, count);
			//Elysia.StaffunLock();
			return ib;
		}
		Ref<Texture2D> OpenGLRendererAPI::GetTexture2D(const TextureSpecification& specification)
		{
			std::unique_lock<std::mutex> SayMyName(Elysia);
			//Elysia.StaffLock();
			auto texture = Texture2D::Create(specification);
			//Elysia.StaffunLock();
			return texture;
		}
		static std::mutex bb;
		Ref<Texture2D> OpenGLRendererAPI::GetTexture2D(const std::string& path)
		{
			std::unique_lock<std::mutex> SayMyName(Elysia);
			//Elysia.StaffLock();
			auto texture = Texture2D::Create(path);
			//Elysia.StaffunLock();
			return texture;
		}
	}
}