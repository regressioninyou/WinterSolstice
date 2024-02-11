#include "k423.h"

#include "WinterSolstice/Render/RendererAPI.h"
namespace WinterSolstice {
	namespace Bronya {
		class OpenGLRendererAPI :public RendererAPI
		{
		public:
			virtual void Init() override;
			virtual void SetClearColor(const glm::vec4& color) override;
			virtual void Clear() override;
			virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
			virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override; 
			virtual void DrawIndex(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count = 0) override;
			virtual void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count = 0) override;
			virtual void DrawIndexLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t count = 0)override;

			virtual void SetLineWidth(float width) override;

			virtual Ref<VertexArray> GetVertexArray() override;
			virtual Ref<VertexBuffer> GetVertexBuffer(uint32_t size) override;
			virtual Ref<VertexBuffer> GetVertexBuffer(float* vertices, uint32_t size) override;
			virtual Ref<VertexBuffer> GetVertexBuffer(void* vertices, uint32_t size) override;
			virtual Ref<VertexBuffer> GetVertexBuffer(const std::vector<Point>& vertices, uint32_t size) override;
			virtual Ref<IndexBuffer> GetIndexBuffer(uint32_t* indices, uint32_t count) override;
			virtual Ref<Texture2D> GetTexture2D(const TextureSpecification& specification) override;
			virtual Ref<Texture2D> GetTexture2D(const std::string& path) override;

		};
	}
}