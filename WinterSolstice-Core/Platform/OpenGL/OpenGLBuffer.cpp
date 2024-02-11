#include "k423.h"
#include "OpenGLBuffer.h"
#include <glad/glad.h>

namespace WinterSolstice {
	namespace Bronya {
		OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
		{
			glCreateBuffers(1, &m_RenderID);
			glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
			glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
		}
		OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
		{
			glCreateBuffers(1, &m_RenderID);
			glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
			glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		}
		OpenGLVertexBuffer::OpenGLVertexBuffer(void* vertices, uint32_t size)
		{
			glCreateBuffers(1, &m_RenderID);
			glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
			glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		}
		OpenGLVertexBuffer::OpenGLVertexBuffer(const std::vector<Point>& vertices, uint32_t size)
		{
			glCreateBuffers(1, &m_RenderID);
			glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
			glBufferData(GL_ARRAY_BUFFER, size, &vertices[0], GL_STATIC_DRAW);
		}
		OpenGLVertexBuffer::~OpenGLVertexBuffer()
		{
			glDeleteBuffers(1, &m_RenderID);
		}
		void OpenGLVertexBuffer::Bind() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
		}
		void OpenGLVertexBuffer::UnBind() const
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		}
		OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
			:m_count(count)
		{
			glCreateBuffers(1, &m_RenderID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
		}
		OpenGLIndexBuffer::~OpenGLIndexBuffer()
		{
			glDeleteBuffers(1, &m_RenderID);
		}
		void OpenGLIndexBuffer::Bind() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
		}
		void OpenGLIndexBuffer::UnBind() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
	}
}