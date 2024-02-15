#pragma once 
namespace WinterSolstice {
	namespace Bronya {
		class Shader;
		class VertexArray;
		class Framebuffer;
	}
	class ScreenPostProcessing
	{
	public:
		ScreenPostProcessing();
		~ScreenPostProcessing();

		void OnResize(uint32_t width, uint32_t height);
		void OnImGuiRender();
		void OnUpdate(Ref<Bronya::Framebuffer> framebuffer);
		uint32_t GetRenderer();


	private:
		struct HDR {
			bool HDR;
			float exposure;
		};
		HDR hdr;
		Ref<Bronya::Shader> Processing_HDRShader;
		Ref<Bronya::Shader> Processing_BloomShader;
		Ref<Bronya::VertexArray> Processing_Screen;
		Ref<Bronya::Framebuffer> Processing_Bloom;
		Ref<Bronya::Framebuffer> Processing_HDR;
	};
}