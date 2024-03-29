#include "k423.h"
#include "OpenGLFramebuffer.h"
#include <glad/glad.h>
#include <array>
namespace WinterSolstice {
	namespace Bronya {
		static const uint32_t s_MaxFramebufferSize = 8192;

		namespace Utils {

			static GLenum TextureTarget(bool multisampled)
			{
				return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			}

			static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
			{
				glCreateTextures(TextureTarget(multisampled), count, outID);
			}

			static void CreateTexture(bool multisampled, uint32_t* outID)
			{
				glCreateTextures(TextureTarget(multisampled), 1, outID);
			}

			static void BindTexture(bool multisampled, uint32_t id)
			{
				glBindTexture(TextureTarget(multisampled), id);
			}

			static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
			{
				bool multisampled = samples > 1;
				if (multisampled)
				{
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
				}
				else
				{
					glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
				// 检查帧缓冲是否完整
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
					std::cerr << "Error: Framebuffer is not complete!" << std::endl;
				}
			}

			static void AttachIntTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
			{
				bool multisampled = samples > 1;
				if (multisampled)
				{
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
				}
				else
				{
					glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_INT, nullptr);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
				// 检查帧缓冲是否完整
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
					std::cerr << "Error: Framebuffer is not complete!" << std::endl;
				}
			}

			static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
			{
				bool multisampled = samples > 1;
				if (multisampled)
				{
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
				}
				else
				{
					glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
				// 检查帧缓冲是否完整
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
					std::cerr << "Error: Framebuffer is not complete!" << std::endl;
				}
			}

			static bool IsDepthFormat(FramebufferTextureFormat format)
			{
				switch (format)
				{
				case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
				}

				return false;
			}

			static GLenum FBTextureFormatToGL(FramebufferTextureFormat format)
			{
				switch (format)
				{
				case FramebufferTextureFormat::RGBA: return GL_RGBA;
				case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
				case FramebufferTextureFormat::RGBA16F: return GL_RGBA16F;
				case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
				}

				Kiana_CORE_ASSERT(false);
				return 0;
			}

		}

		OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
			: m_Specification(spec)
		{
			for (auto spec : m_Specification.Attachments.Attachments)
			{
				if (!Utils::IsDepthFormat(spec.TextureFormat))
					m_ColorAttachmentSpecifications.emplace_back(spec);
				else
					m_DepthAttachmentSpecification = spec;
			}

			Invalidate();
		}

		OpenGLFramebuffer::~OpenGLFramebuffer()
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);
		}

		void OpenGLFramebuffer::Invalidate()
		{
			if (m_RendererID)
			{
				glDeleteFramebuffers(1, &m_RendererID);
				glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
				glDeleteTextures(1, &m_DepthAttachment);

				m_ColorAttachments.clear();
				m_DepthAttachment = 0;
			}

			glCreateFramebuffers(1, &m_RendererID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

			bool multisample = m_Specification.Samples > 1;

			// Attachments
			if (m_ColorAttachmentSpecifications.size())
			{
				m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
				Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

				for (size_t i = 0; i < m_ColorAttachments.size(); i++)
				{
					Utils::BindTexture(multisample, m_ColorAttachments[i]);
					switch (m_ColorAttachmentSpecifications[i].TextureFormat)
					{
					case FramebufferTextureFormat::RGBA:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					case FramebufferTextureFormat::RGBA8:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					case FramebufferTextureFormat::RGBA16F:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA16F, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					case FramebufferTextureFormat::RED_INTEGER:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
						break;
					}
				}
			}

			if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
			{
				Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
				Utils::BindTexture(multisample, m_DepthAttachment);
				switch (m_DepthAttachmentSpecification.TextureFormat)
				{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
				}
			}

			if (m_ColorAttachments.size() > 1)
			{
				Kiana_CORE_ASSERT(m_ColorAttachments.size() <= 5);
				GLenum buffers[5] = { GL_COLOR_ATTACHMENT0,
					GL_COLOR_ATTACHMENT1,
					GL_COLOR_ATTACHMENT2,
					GL_COLOR_ATTACHMENT3,
					GL_COLOR_ATTACHMENT4 };
				glDrawBuffers(m_ColorAttachments.size(), buffers);
				//glDrawBuffers(m_ColorAttachments.size(), buffers);

				//Kiana_CORE_ASSERT(m_ColorAttachments.size() <= 4);
				//GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				//glDrawBuffers(m_ColorAttachments.size(), buffers);
			}
			else if (m_ColorAttachments.empty())
			{
				// Only depth-pass
				glDrawBuffer(GL_NONE);
			}

			Kiana_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void OpenGLFramebuffer::Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
			glViewport(0, 0, m_Specification.Width, m_Specification.Height);
		}

		void OpenGLFramebuffer::Unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
		{
			if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
			{
				Kiana_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
				return;
			}
			m_Specification.Width = width;
			m_Specification.Height = height;

			Invalidate();
		}

		int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
		{
			Kiana_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

			glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
			int pixelData;
			glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
			return pixelData;

		}

		void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
		{
			Kiana_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

			auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
			glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
				Utils::FBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
		}
		void OpenGLFramebuffer::BindTextureID(FramebufferTextureFormat format, uint32_t slot)
		{
			switch (format)
			{
			case WinterSolstice::Bronya::FramebufferTextureFormat::None:
				glBindTextureUnit(slot, m_DepthAttachment);
				break;
			case WinterSolstice::Bronya::FramebufferTextureFormat::RGBA:
				glBindTextureUnit(slot, m_ColorAttachments[0]);
				break;
			case WinterSolstice::Bronya::FramebufferTextureFormat::RGBA8:
				glBindTextureUnit(slot, m_DepthAttachment);
				break;
			case WinterSolstice::Bronya::FramebufferTextureFormat::RGBA16F:
				glBindTextureUnit(slot, m_DepthAttachment);
				break;
			case WinterSolstice::Bronya::FramebufferTextureFormat::RED_INTEGER:
				glBindTextureUnit(slot, m_DepthAttachment);
				break;
			case WinterSolstice::Bronya::FramebufferTextureFormat::DEPTH24STENCIL8:
				glBindTextureUnit(slot, m_DepthAttachment);
				break;
			default:
				break;
			}
		}
		void OpenGLFramebuffer::BindColorAttachments()
		{
			for (uint32_t i = 0; i < m_ColorAttachments.size(); i++) {
				glBindTextureUnit(i, m_ColorAttachments[i]);
			}
		}
		void OpenGLFramebuffer::BindColorAttachments(uint32_t slot, uint32_t index)
		{
			glBindTextureUnit(slot, m_ColorAttachments[index]);
		}
		void OpenGLFramebuffer::UnBindTextureID(FramebufferTextureFormat format, uint32_t slot)
		{
			uint32_t i = 0;
			for (auto& id : m_ColorAttachments) {
				glBindTextureUnit(i, 0);
				i++;
			}
		}
		uint32_t OpenGLFramebuffer::GetRendererID()
		{
			return m_RendererID;
		}
		void OpenGLFramebuffer::BindReadBuffer(uint32_t slot)
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, slot);
		}
		void OpenGLFramebuffer::BindDrawBuffer(uint32_t slot)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, slot); // write to default framebuffer
		}
		void OpenGLFramebuffer::ToOtherFramebuffer(uint32_t slot)
		{
			BindReadBuffer(m_RendererID);
			BindDrawBuffer(slot);
			// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
			// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
			// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
			glBlitFramebuffer(0, 0, m_Specification.Width, m_Specification.Height,
				0, 0, m_Specification.Width, m_Specification.Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBlitFramebuffer(0, 0, m_Specification.Width, m_Specification.Height,
				0, 0, m_Specification.Width, m_Specification.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void OpenGLFramebuffer::ToOtherFramebuffer(uint32_t slot, const FramebufferSpecification& slotFormat)
		{
			//BindReadBuffer(m_RendererID);
			//BindDrawBuffer(slot);
			// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
			// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
			// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
			glBlitFramebuffer(0, 0, m_Specification.Width, m_Specification.Height,
				0, 0, slotFormat.Width, slotFormat.Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void OpenGLFramebuffer::BindTextureID(uint32_t slot, uint32_t index)
		{
			glBindTextureUnit(slot, m_ColorAttachments[index]);
		}
		MultiLevelFramebuffer::MultiLevelFramebuffer(const FramebufferSpecification& spec, uint32_t level)
			:level(level), m_Specification(spec)
		{
			for (auto spec : m_Specification.Attachments.Attachments)
			{
				if (!Utils::IsDepthFormat(spec.TextureFormat))
					m_ColorAttachmentSpecifications.emplace_back(spec);
				else
					m_DepthAttachmentSpecification = spec;
			}
			Invalidate();
		}
		MultiLevelFramebuffer::~MultiLevelFramebuffer()
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);
		}
		void MultiLevelFramebuffer::Invalidate()
		{
			if (m_RendererID)
			{
				glDeleteFramebuffers(1, &m_RendererID);
				glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
				glDeleteTextures(1, &m_DepthAttachment);

				m_ColorAttachments.clear();
				m_DepthAttachment = 0;
			}

			glCreateFramebuffers(1, &m_RendererID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

			bool multisample = m_Specification.Samples > 1;

			// Attachments
			if (m_ColorAttachmentSpecifications.size())
			{
				//m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size() * level);
				m_ColorAttachments.clear();

				//for (size_t i = 0; i < m_ColorAttachments.size(); i++)
				{
					int width = m_Specification.Width / 2;
					int height = m_Specification.Width / 2;
					switch (m_ColorAttachmentSpecifications[0].TextureFormat)
					{
					case FramebufferTextureFormat::RGBA:
						for (size_t j = 0; j < level; j++) {
							if (width / 2 > 10 || height / 2 > 10)
								Utils::AttachColorTexture(m_ColorAttachments[j], m_Specification.Samples,
									GL_RGBA, GL_RGBA, width / 2, height / 2, j);
						}
						break;
					case FramebufferTextureFormat::RGBA8:
						for (size_t j = 0; j < level; j++) {
							if (width / 2 > 10 || height / 2 > 10)
								Utils::AttachColorTexture(m_ColorAttachments[j], m_Specification.Samples,
									GL_RGBA8, GL_RGBA, width / 2, height / 2, j);
						}
						break;
					case FramebufferTextureFormat::RGBA16F:
						//Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());
						for (size_t j = 0; j < level; j++) {
							if (width > 10 || height > 10) {
								uint32_t id = 0;
								Utils::CreateTexture(multisample,&id);
								Utils::BindTexture(multisample, id);
								Utils::AttachColorTexture(id, m_Specification.Samples,
									GL_RGBA16F, GL_RGBA, width, height, j);
								m_ColorAttachments.push_back(id);
							}
							width /= 2;
							height /= 2;
						}
						break;
					case FramebufferTextureFormat::RED_INTEGER:
						for (size_t j = 0; j < level; j++) {
							if (width / 2 > 10 || height / 2 > 10)
								Utils::AttachColorTexture(m_ColorAttachments[j], m_Specification.Samples,
									GL_R32I, GL_RED_INTEGER, width / 2, height / 2, j);
						}
						break;
					}
				}
			}

			if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
			{
				Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
				Utils::BindTexture(multisample, m_DepthAttachment);
				switch (m_DepthAttachmentSpecification.TextureFormat)
				{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
				}
			}

			if (m_ColorAttachments.size() > 1)
			{
				Kiana_CORE_ASSERT(m_ColorAttachments.size() <= level);
				std::vector<GLenum> buffers;
				for (uint32_t i = 0; i < level; i++) {
					buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
				}
				glDrawBuffers(m_ColorAttachments.size(), buffers.data());
				//glDrawBuffers(m_ColorAttachments.size(), buffers);

				//Kiana_CORE_ASSERT(m_ColorAttachments.size() <= 4);
				//GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				//glDrawBuffers(m_ColorAttachments.size(), buffers);
			}
			else if (m_ColorAttachments.empty())
			{
				// Only depth-pass
				glDrawBuffer(GL_NONE);
			}

			Kiana_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void MultiLevelFramebuffer::Bind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
			glViewport(0, 0, m_Specification.Width, m_Specification.Height);
		}
		void MultiLevelFramebuffer::Unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void MultiLevelFramebuffer::Resize(uint32_t width, uint32_t height)
		{
			if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
			{
				Kiana_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
				return;
			}
			m_Specification.Width = width;
			m_Specification.Height = height;

			Invalidate();
		}
		int MultiLevelFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
		{
			Kiana_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

			glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
			int pixelData;
			glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
			return pixelData;
		}
		void MultiLevelFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
		{
			Kiana_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

			auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
			glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
				Utils::FBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
		}
		uint32_t MultiLevelFramebuffer::GetColorAttachmentRendererID(uint32_t index) const
		{
			return m_ColorAttachments[index];
		}
		uint32_t MultiLevelFramebuffer::GetDepthAttachmentRendererID(uint32_t index) const
		{
			return m_DepthAttachment;
		}
		const FramebufferSpecification& MultiLevelFramebuffer::GetSpecification() const
		{
			return m_Specification;
		}
		void MultiLevelFramebuffer::BindTextureID(uint32_t slot, uint32_t index)
		{
			glBindTextureUnit(slot, m_ColorAttachments[index]);
		}
		void MultiLevelFramebuffer::BindTextureID(FramebufferTextureFormat format, uint32_t slot)
		{
			switch (format)
			{
			case WinterSolstice::Bronya::FramebufferTextureFormat::None:
				glBindTextureUnit(slot, m_DepthAttachment);
				break;
			case WinterSolstice::Bronya::FramebufferTextureFormat::RGBA:
				glBindTextureUnit(slot, m_ColorAttachments[0]);
				break;
			case WinterSolstice::Bronya::FramebufferTextureFormat::RGBA8:
				glBindTextureUnit(slot, m_DepthAttachment);
				break;
			case WinterSolstice::Bronya::FramebufferTextureFormat::RGBA16F:
				glBindTextureUnit(slot, m_DepthAttachment);
				break;
			case WinterSolstice::Bronya::FramebufferTextureFormat::RED_INTEGER:
				glBindTextureUnit(slot, m_DepthAttachment);
				break;
			case WinterSolstice::Bronya::FramebufferTextureFormat::DEPTH24STENCIL8:
				glBindTextureUnit(slot, m_DepthAttachment);
				break;
			default:
				break;
			}
		}
		void MultiLevelFramebuffer::BindColorAttachments()
		{
			for (uint32_t i = 0; i < m_ColorAttachments.size(); i++) {
				glBindTextureUnit(i, m_ColorAttachments[i]);
			}
		}
		void MultiLevelFramebuffer::BindColorAttachments(uint32_t slot, uint32_t index)
		{
			glBindTextureUnit(slot, m_ColorAttachments[index]);
		}
		void MultiLevelFramebuffer::UnBindTextureID(FramebufferTextureFormat format, uint32_t slot)
		{
			uint32_t i = 0;
			for (auto& id : m_ColorAttachments) {
				glBindTextureUnit(i, 0);
				i++;
			}
		}
		uint32_t MultiLevelFramebuffer::GetRendererID()
		{
			return m_RendererID;
		}
		void MultiLevelFramebuffer::ToOtherFramebuffer(uint32_t slot)
		{
			BindReadBuffer(m_RendererID);
			BindDrawBuffer(slot);
			// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
			// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
			// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
			glBlitFramebuffer(0, 0, m_Specification.Width, m_Specification.Height,
				0, 0, m_Specification.Width, m_Specification.Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBlitFramebuffer(0, 0, m_Specification.Width, m_Specification.Height,
				0, 0, m_Specification.Width, m_Specification.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		void MultiLevelFramebuffer::ToOtherFramebuffer(uint32_t slot, const FramebufferSpecification& slotFormat)
		{
			glBlitFramebuffer(0, 0, m_Specification.Width, m_Specification.Height,
				0, 0, slotFormat.Width, slotFormat.Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		}
		void MultiLevelFramebuffer::BindReadBuffer(uint32_t slot)
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, slot);
		}
		void MultiLevelFramebuffer::BindDrawBuffer(uint32_t slot)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, slot); // write to default framebuffer
		}
	}
}