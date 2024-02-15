#pragma once

#include "WinterSolstice/Core/Core.h"

namespace WinterSolstice {
	namespace Bronya {

		enum class FramebufferTextureFormat
		{
			None = 0,

			// Color
			RGBA,
			RGBA8,
			RGBA16F,
			RED_INTEGER,


			// Depth/stencil
			DEPTH24STENCIL8,

			// Defaults
			Depth = DEPTH24STENCIL8
		};

		struct FramebufferTextureSpecification
		{
			FramebufferTextureSpecification() = default;
			FramebufferTextureSpecification(FramebufferTextureFormat format)
				: TextureFormat(format)
				//,Name("None") 
			{}
			FramebufferTextureSpecification(FramebufferTextureFormat format,const std::string& name)
				: TextureFormat(format)
				//,Name(name) 
			{}

			//std::string Name;
			FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
			// TODO: filtering/wrap
		};

		struct FramebufferAttachmentSpecification
		{
			FramebufferAttachmentSpecification() = default;
			FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
				: Attachments(attachments) {}

			std::vector<FramebufferTextureSpecification> Attachments;
		};

		struct FramebufferSpecification
		{
			uint32_t Width = 0, Height = 0;
			FramebufferAttachmentSpecification Attachments;
			uint32_t Samples = 1;

			bool SwapChainTarget = false;
		};

		class Framebuffer
		{
		public:
			virtual ~Framebuffer() = default;

			virtual void Bind() = 0;
			virtual void Unbind() = 0;

			virtual void Resize(uint32_t width, uint32_t height) = 0;
			virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

			virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

			virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
			virtual uint32_t GetDepthAttachmentRendererID(uint32_t index = 0) const = 0;

			virtual const FramebufferSpecification& GetSpecification() const = 0;

			virtual void BindTextureID(FramebufferTextureFormat format, uint32_t slot) = 0;
			virtual void BindColorAttachments() = 0;
			virtual void BindColorAttachments(uint32_t slot,uint32_t index) = 0;
			virtual void UnBindTextureID(FramebufferTextureFormat format, uint32_t slot) = 0;

			virtual uint32_t GetRendererID() = 0;
			virtual void ToOtherFramebuffer(uint32_t slot = 0) = 0;
			virtual void ToOtherFramebuffer(uint32_t slot,const FramebufferSpecification& slotFormat) = 0;

			virtual void BindReadBuffer(uint32_t slot) = 0;
			virtual void BindDrawBuffer(uint32_t slot) = 0;

			static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
		};


	}
}