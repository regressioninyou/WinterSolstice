#include "k423.h"
#include "ScreenPostProcessing.h"
#include <WinterSolstice/Render/Shader.h>
#include <WinterSolstice/Render/Buffer.h>
#include <WinterSolstice/Render/VertexArray.h>
#include <WinterSolstice/Render/Framebuffer.h>
#include <WinterSolstice/Core/Application.h>
#include <WinterSolstice/Render/RendererCommand.h>
#include <WinterSolstice/Render/Renderer.h>
#include <WinterSolstice/Render/Renderer2D.h>
#include <imgui_internal.h>
#define SHADERSOURCE(x) std::string("T:/orther/main/c++/WinterSolstice/WinterSolstice-Editory/assets/shaders/").append(x)
namespace WinterSolstice {
	ScreenPostProcessing::ScreenPostProcessing()
		:hdr({ false,1.0f })
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		uint32_t offset = 0;

		uint32_t quadIndices[] = { 0, 1, 2,1, 3, 2 };
		Processing_Screen = Bronya::VertexArray::Create();
		auto vabuffer = Bronya::VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		{
			Bronya::BufferLayout layer = {
				{ Bronya::ShaderDataType::Float3,"aPosition"},
				{ Bronya::ShaderDataType::Float2,"aTexCoord"}
			};
			vabuffer->SetLayout(layer);
		}
		auto idsbuffer = Bronya::IndexBuffer::Create(quadIndices, 6);
		Processing_Screen->AddVertexBuffer(vabuffer);
		Processing_Screen->SetIndexBuffer(idsbuffer);
		Processing_HDRShader = Bronya::Shader::Create(SHADERSOURCE("HDR/HDR.vs"), SHADERSOURCE("HDR/HDR.fs"));

		Bronya::FramebufferSpecification HDR;
		HDR.Attachments = {
			Bronya::FramebufferTextureFormat::RGBA16F,
			Bronya::FramebufferTextureFormat::RGBA16F,
			Bronya::FramebufferTextureFormat::Depth
		};
		HDR.Width = 1600.0f;
		HDR.Height = 900.0f;
		Processing_HDR = Bronya::Framebuffer::Create(HDR);
		Bronya::FramebufferSpecification bloom;
		bloom.Attachments = {
			Bronya::FramebufferTextureFormat::RGBA16F,
			Bronya::FramebufferTextureFormat::Depth
		};
		bloom.Width = 1600.0f;
		bloom.Height = 900.0f;
		Processing_Bloom = Bronya::Framebuffer::Create(bloom);
		Processing_BloomShader = Bronya::Shader::Create(SHADERSOURCE("HDR/Blur.vs"), SHADERSOURCE("HDR/Blur.fs"));
	}
	ScreenPostProcessing::~ScreenPostProcessing()
	{
	}
	void ScreenPostProcessing::OnResize(uint32_t width, uint32_t height)
	{
		Application::Get().RenderThread()->AddTaskAwait([this, width, height]() {
			Processing_HDR->Resize(width, height);
			Processing_Bloom->Resize(width, height);
			});
	}
	void ScreenPostProcessing::OnImGuiRender()
	{
		ImGui::Begin("Processing");
		ImGui::Text("HDR");
		ImGui::Checkbox("HDR", &hdr.HDR);
		ImGui::DragFloat("Exposure", &hdr.exposure, 0.01f, 0.0f, 10.0f,"%.1f");
		ImGui::End();
	}
	void ScreenPostProcessing::OnUpdate(Ref<Bronya::Framebuffer> framebuffer)
	{
		auto HDR = [this, framebuffer]() {
			Processing_HDR->Bind();
			Bronya::RenderCommand::SetClearColor(glm::vec4{ 0.1f, 0.1f, 0.1f, 1.0f });
			Bronya::RenderCommand::Clear();
			Bronya::Renderer::ResetStats();
			auto RendererScreen = [this, framebuffer]() {
				Processing_HDRShader->Bind();
				Processing_HDRShader->SetBool("hdr", hdr.HDR);
				Processing_HDRShader->SetFloat("exposure", hdr.exposure);
				framebuffer->BindColorAttachments(0, 0);
				framebuffer->BindColorAttachments(1, 2);
				};
			Bronya::Renderer::DrawScreen(Processing_Screen, std::move(RendererScreen));

			Processing_HDR->Unbind();
			};
		Application::Get().RenderThread()->AddTaskAwait(HDR);
		auto Bloom = [this]()
			{
				Processing_Bloom->Bind();
				Bronya::RenderCommand::SetClearColor(glm::vec4{ 0.1f, 0.1f, 0.1f, 1.0f });
				Bronya::RenderCommand::Clear();
				auto RendererScreen = [this]() {
					Processing_BloomShader->Bind();
					Processing_HDR->BindColorAttachments();
					};
				Bronya::Renderer::DrawScreen(Processing_Screen, std::move(RendererScreen));
				Processing_Bloom->Unbind();
			};
		Application::Get().RenderThread()->AddTaskAwait(Bloom); //
	}
	uint32_t ScreenPostProcessing::GetRenderer()
	{
		return Processing_HDR->GetColorAttachmentRendererID(1);
	}
}