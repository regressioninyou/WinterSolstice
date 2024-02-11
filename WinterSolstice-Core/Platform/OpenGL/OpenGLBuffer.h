#pragma once
#include "WinterSolstice/Render/Buffer.h"
#include <WinterSolstice/Core/StructBase.h>
namespace WinterSolstice {
	namespace Bronya {
		class OpenGLVertexBuffer : public VertexBuffer
		{
		public:
			OpenGLVertexBuffer(uint32_t size);
			OpenGLVertexBuffer(float* vertices, uint32_t size);
			OpenGLVertexBuffer(void* vertices, uint32_t size);
			OpenGLVertexBuffer(const std::vector<Point>& vertices, uint32_t size);
			~OpenGLVertexBuffer();

			virtual void Bind()const;
			virtual void UnBind()const;
			virtual void SetData(const void* data, uint32_t size);
			virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; };
			virtual BufferLayout& GetLayout()override { return m_Layout; };
		private:
			uint32_t m_RenderID;
			BufferLayout m_Layout;
		};
		class OpenGLIndexBuffer : public IndexBuffer
		{
		public:
			OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
			~OpenGLIndexBuffer();

			virtual void Bind()const;
			virtual void UnBind()const;

			virtual uint32_t GetCount() const { return m_count; };
		private:
			uint32_t m_RenderID;
			uint32_t m_count;
		};
	}
}