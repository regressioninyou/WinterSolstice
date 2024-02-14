#pragma once 
#include "WinterSolstice/Render/Framebuffer.h"

namespace WinterSolstice {
	namespace Bronya {

		class OpenGLFramebuffer : public Framebuffer
		{
		public:
			OpenGLFramebuffer(const FramebufferSpecification& spec);
			virtual ~OpenGLFramebuffer();

			void Invalidate();

			virtual void Bind() override;
			virtual void Unbind() override;

			virtual void Resize(uint32_t width, uint32_t height) override;
			virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

			virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

			virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { Kiana_CORE_ASSERT(index < m_ColorAttachments.size()); return m_ColorAttachments[index]; }

			virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

			virtual void BindTextureID(FramebufferTextureFormat format, uint32_t slot) override;
			virtual void BindColorAttachments() override;
			virtual void UnBindTextureID(FramebufferTextureFormat format, uint32_t slot) override;

			virtual uint32_t GetRendererID()override;
			virtual void BindReadBuffer(uint32_t slot)override;
			virtual void BindDrawBuffer(uint32_t slot)override;
			virtual void ToOtherFramebuffer(uint32_t slot = 0) override;
			virtual void ToOtherFramebuffer(uint32_t slot,const FramebufferSpecification& slotFormat) override;

			virtual uint32_t GetDepthAttachmentRendererID(uint32_t index = 0) const override
			{
				Kiana_CORE_ASSERT(index < m_ColorAttachments.size()); return m_DepthAttachment;
			}
		private:
			uint32_t m_RendererID = 0;
			FramebufferSpecification m_Specification;

			std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
			FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

			std::vector<uint32_t> m_ColorAttachments;
			uint32_t m_DepthAttachment = 0;
		};

	}
}