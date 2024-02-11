#pragma once 
#include "RendererAPI.h"
#include <WinterSolstice/Core/True.h>
namespace WinterSolstice {
	namespace Bronya {
		class RenderCommand
		{
		public:
			inline static void Init() {
				s_RendererAPI->Init();
			};
			inline static void SetClearColor(const glm::vec4& color)
			{
				s_RendererAPI->SetClearColor(color);
			};

			inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
				s_RendererAPI->SetViewport(x, y, width, height);
			}

			inline static void Clear()
			{
				s_RendererAPI->Clear();
			};

			inline static void DrawIndex(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count = 0) {
				s_RendererAPI->DrawIndex(vertexArray, count);
			};
			inline static void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count) {
				s_RendererAPI->DrawArrays(vertexArray, count);
			};

			inline static void DrawIndexLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count = 0) {
				s_RendererAPI->DrawIndexLines(vertexArray, count);
			}

			inline static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
			{
				s_RendererAPI->DrawLines(vertexArray, vertexCount);
			}

			inline static void SetLineWidth(float width)
			{
				s_RendererAPI->SetLineWidth(width);
			}

			inline static Ref<VertexArray> GetVertexArray() {
				return s_RendererAPI->GetVertexArray();
			}

			inline static Ref<VertexBuffer> GetVertexBuffer(uint32_t size) {
				return s_RendererAPI->GetVertexBuffer(size);
			}

			inline static Ref<VertexBuffer> GetVertexBuffer(void* vertices, uint32_t size) {
				return s_RendererAPI->GetVertexBuffer(vertices, size);
			}

			inline static Ref<VertexBuffer> GetVertexBuffer(const std::vector<Point>& vertices, uint32_t size) {
				return s_RendererAPI->GetVertexBuffer(vertices, size);
			}
			inline static Ref<IndexBuffer> GetIndexBuffer(uint32_t* indices, uint32_t count) {
				return s_RendererAPI->GetIndexBuffer(indices, count);
			}
			inline static Ref<Texture2D> GetTexture2D(const TextureSpecification& specification) {
				return s_RendererAPI->GetTexture2D(specification);
			}
			inline static Ref<Texture2D> GetTexture2D(const std::string& path) {
				return s_RendererAPI->GetTexture2D(path);
			}

		private:
			static RendererAPI* s_RendererAPI;
		};
	}
}