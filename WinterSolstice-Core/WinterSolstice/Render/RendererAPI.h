#pragma once 
#include "WinterSolstice/Render/VertexArray.h"
#include <glm/glm.hpp>
#include "Texture.h"

namespace WinterSolstice {
	namespace Bronya {
		class RendererAPI
		{
		public:
			enum class API
			{
				None = 0, OpenGL = 1
			};
		public:
			virtual void Init() = 0;
			virtual void SetClearColor(const glm::vec4& color) = 0;
			virtual void Clear() = 0;
			virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
			virtual void DrawIndex(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count = 0) = 0;
			virtual void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count = 0) = 0;
			virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
			virtual void DrawIndexLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) = 0;
			virtual void SetLineWidth(float width) = 0;
			virtual Ref<VertexArray> GetVertexArray() = 0;

			virtual Ref<VertexBuffer> GetVertexBuffer(uint32_t size) = 0;

			virtual Ref<VertexBuffer> GetVertexBuffer(float* vertices, uint32_t size) = 0;

			virtual Ref<VertexBuffer> GetVertexBuffer(void* vertices, uint32_t size) = 0;

			virtual Ref<VertexBuffer> GetVertexBuffer(const std::vector<Point>& vertices, uint32_t size) = 0;
			virtual Ref<IndexBuffer> GetIndexBuffer(uint32_t* indices, uint32_t count) = 0;
			virtual Ref<Texture2D> GetTexture2D(const TextureSpecification& specification) = 0;
			virtual Ref<Texture2D> GetTexture2D(const std::string& path) = 0;

			inline static API GetAPI() { return s_API; };
		private:
			static API s_API;
		};
	}
}