#include <KyBao.h>
#include "TestLoader.h"
#include <glad/glad.h>
#if 0
class ExampleLayer :public KyBao::Layer
{
public:
	ExampleLayer() :
		Layer("Example")
	{
		m_Camera = KyBao::EditorCamera::EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		m_ShaderLibrary = KyBao::CreateRef<KyBao::ShaderLibrary>();
		std::string TvsSrc = R"(
#version 450 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;

uniform mat4 uViewProjection;
uniform mat4 uModel;
out vec2 vTexcoord;
void main (){
	vTexcoord = aTexcoord;
	gl_Position =   uViewProjection * vec4( aPosition,1.0f); 
}
		)";
		std::string TfsSrc = R"(
#version 330 core
out vec4 FragColor;

in vec2 vTexcoord;

uniform sampler2D texture_diffuse1;

void main()
{    
    FragColor = texture(texture_diffuse1, vTexcoord);
}
)";
		m_ShaderLibrary->Add("Tirangle", KyBao::Shader::Create("./assets/shaders/Model_vs.glsl", "./assets/shaders/Model_fs.glsl"));
		m_ShaderLibrary->Add("Blue", KyBao::Shader::Create(TvsSrc, TfsSrc));
		KyBao::FramebufferSpecification fbsf;
		fbsf.Width = 1600;
		fbsf.Height = 900;
		//m_Framebuffer = KyBao::Framebuffer::Create(fbsf);
		//obj = KyBao::Loder::Get()->loadOBJ("T:/orther/main/c++/Project2/assets/nanosuit/nanosuit.obj");
		//obj = KyBao::Loder::Get()->loadOBJ("C:/Users/sa/Desktop/Assest/just-a-girl/source/final_v01.obj");
		//obj = KyBao::Loder::Get()->loadOBJ("C:/Users/sa/Desktop/Assest/untitled.obj");

		//bvh.buildBVH(obj->GetOBJ());
		//m.loadModel("C:/Users/sa/Desktop/Assest/q版爱莉希雅_by_神帝宇_abee716a2ce8b85903db03e56900e09e/q版爱莉希雅/q版爱莉希雅.pmx");
		m.loadModel("T:/orther/main/c++/Project2/assets/nanosuit/nanosuit.obj");
		
	};
	void OnUpdate(KyBao::Timestep ts) override {

		m_Camera.OnUpdate(ts);
		KyBao::RenderCommand::SetClearColor({ 0.1, 0.1, 0.15, 1 });
		KyBao::RenderCommand::Clear();
		//m_Framebuffer->Bind();

		KyBao::Renderer::BeginScene(m_Camera);
		for (auto& mesh : m.meshes) {
			m_ShaderLibrary->Get("Blue")->Bind();
			mesh->BindTexture(m_ShaderLibrary->Get("Blue"));
			KyBao::Renderer::Submit(mesh->getVertexArray(), m_ShaderLibrary->Get("Blue"));

			m_ShaderLibrary->Get("Tirangle")->Bind();
			KyBao::Renderer::SubmitLine(mesh->getVertexArray(), m_ShaderLibrary->Get("Tirangle"));
		}
		//KyBao::Renderer::Submit(m_VertexArray, m_Shader);
		//for (auto elemnt : *obj)
		//{
		//	KyBao::Mesh* mesh = dynamic_cast<KyBao::Mesh*>(elemnt.get());
		//	mesh->BindTexture(m_ShaderLibrary->Get("Blue"));
		//	KyBao::Renderer::Submit(mesh->getVertexArray(), m_ShaderLibrary->Get("Blue"));
		//	KyBao::Renderer::SubmitLine(mesh->getVertexArray(), m_ShaderLibrary->Get("Tirangle"));
		//}
		//KyBao::Renderer2D::BeginScene(m_Camera);
		//bvh.DrawBounds();
		//KyBao::Renderer2D::EndScene();
		//KyBao::Renderer::Submit(m_SquareVA, m_ShaderLibrary->Get("Blue"));
		KyBao::Renderer::EndScene();
		//m_Framebuffer->Unbind();
	}
	virtual void OnImGuiRender() override {
		return;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static bool p_open = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				if (ImGui::MenuItem("Exit"))
					KyBao::Application::Get().Close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::Begin("Render");
		uint32_t renderID = m_Framebuffer->GetColorAttachmentRendererID();
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		//m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
		ImGui::Image((void*)renderID, viewportPanelSize, ImVec2{ 0,1 }, ImVec2{ 1,0 });
		ImGui::End();
		ImGui::End();
	}
	void OnEvent(KyBao::Event& event)override {
		m_Camera.OnEvent(event);

		KyBao::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KyBao::MouseButtonPressedEvent>(BIND_EVENT_FN(ExampleLayer::MouseButtonPressedEvent));
	}
	bool MouseButtonPressedEvent(KyBao::MouseButtonPressedEvent& e) {
		//if (e.GetMouseButton() == KyBao::Mouse::ButtonLeft)
		//{
		//	ImVec2 mousePos = ImGui::GetMousePos();
		//	glm::vec2 windowPos = KyBao::Application::Get().GetWindowPos();
		//	glm::vec2 mouse = { mousePos.x - windowPos.x ,mousePos.y - windowPos.y };
		//	mouse = { (mouse.x - 800.0f),mouse.y - 450.0f };
		//	float ndcX = (2.0f * mouse.x) / 1600 - 1.0f;
		//	float ndcY = 1.0f - (2.0f * mouse.y) / 900;
		//	glm::vec4 clipCoords(ndcX, ndcY, -1.0f, 1.0f);

		//	//glm::mat4 inverseProjectionMatrix = glm::inverse(m_Camera.GetCamera().GetProjectMatrix());
		//	//glm::vec4 eyeCoords = inverseProjectionMatrix * clipCoords;
		//	eyeCoords.z = -1.0f;  // 设置为 -1.0f，表示射线的方向是沿着摄像机观察方向
		//	eyeCoords.w = 0.0f;
		//	//glm::mat4 inverseViewMatrix = glm::inverse(m_Camera.GetCamera().GetViewMatrix());
		//	//glm::vec4 rayWorld = inverseViewMatrix * eyeCoords;
		//	glm::normalize(rayWorld);

		//	KyBao::Ray ray({ eyeCoords.x,eyeCoords.y,eyeCoords.z }, { rayWorld.x,rayWorld.y,rayWorld.z });
		//	std::cout << "x :" << eyeCoords.x << " y :" << eyeCoords.y << " z :" << eyeCoords.z << "\n";
		//	std::cout << "rayWorldx :" << rayWorld.x << " rayWorldy :" << rayWorld.y << " rayWorldz :" << rayWorld.z << "\n";
		//	KyBao::Model* model = dynamic_cast<KyBao::Model*>(obj.get());

		//	if (bvh.IntersectP(ray))
		//	{
		//		std::cout << "click" << "\n";
		//	}
		//	else
		//	{
		//		std::cout << "no click" << "\n";
		//	}
		//}
		return false;
	}
private:
	KyBao::Ref<KyBao::ShaderLibrary> m_ShaderLibrary;

	KyBao::EditorCamera m_Camera;

	KyBao::Ref<KyBao::Framebuffer> m_Framebuffer;

	KyBao::Ref<KyBao::Object> obj;

	KyBao::TestModel m;

	KyBao::BVHAccel bvh;
};
#endif
#if 0
class ExampleLayer :public KyBao::Layer
{
public:
	ExampleLayer() :
		Layer("Example")
	{
		m_Camera = KyBao::EditorCamera::EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		m_ShaderLibrary = KyBao::CreateRef<KyBao::ShaderLibrary>();
		std::string TvsSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;

uniform mat4 uViewProjection;
uniform mat4 uModel;
out vec2 vTexcoord;
void main (){
	vTexcoord = aTexcoord;
	gl_Position =   uViewProjection * vec4( aPosition,1.0f); 
}
		)";
		std::string TfsSrc = R"(
#version 330 core
out vec4 FragColor;

in vec2 vTexcoord;

uniform sampler2D texture_diffuse1;

void main()
{    
    FragColor = texture(texture_diffuse1, vTexcoord);
}
)";
		m_ShaderLibrary->Add("Tirangle", KyBao::Shader::Create("./assets/shaders/Model_vs.glsl", "./assets/shaders/Model_fs.glsl"));
		m_ShaderLibrary->Add("Blue", KyBao::Shader::Create(TvsSrc, TfsSrc));
		KyBao::FramebufferSpecification fbsf;
		fbsf.Width = 1600;
		fbsf.Height = 900;
		//m_Framebuffer = KyBao::Framebuffer::Create(fbsf);
		//obj = KyBao::Loder::Get()->loadOBJ("T:/orther/main/c++/Project2/assets/nanosuit/nanosuit.obj");
		//obj = KyBao::Loder::Get()->loadOBJ("C:/Users/sa/Desktop/Assest/just-a-girl/source/final_v01.obj");
		//obj = KyBao::Loder::Get()->loadOBJ("C:/Users/sa/Desktop/Assest/untitled.obj");

		//bvh.buildBVH(obj->GetOBJ());
		m.loadModel("C:/Users/sa/Desktop/Assest/q版爱莉希雅_by_神帝宇_abee716a2ce8b85903db03e56900e09e/q版爱莉希雅/q版爱莉希雅.pmx");
		m.loadModel("C:/Users/sa/Desktop/Assest/爱莉希雅律者2.0_by_神帝宇_1e1281846d381fb4553fcc5b5a6cdfbf/爱莉希雅律者2.0/爱莉希雅 3.0.pmx");
		//m.loadModel("T:/orther/main/c++/Project2/assets/nanosuit/nanosuit.obj");
		KyBao::Application::Get().GetWindow().SetVSync(0);

	};
	void OnEvent(KyBao::Event & event)override {
		m_Camera.OnEvent(event);
	}
	void OnUpdate(KyBao::Timestep ts) override {

		m_Camera.OnUpdate(ts);
		KyBao::RenderCommand::SetClearColor({ 0.1, 0.1, 0.15, 1 });
		KyBao::RenderCommand::Clear();
		//m_Framebuffer->Bind();

		KyBao::Renderer::BeginScene(m_Camera);
		for (auto& mesh : m.meshes) {
			m_ShaderLibrary->Get("Blue")->Bind();
			mesh->BindTexture(m_ShaderLibrary->Get("Blue"));
			KyBao::Renderer::Submit(mesh->getVertexArray(), m_ShaderLibrary->Get("Blue"));
		}
	}
	virtual void OnImGuiRender() override {

		ImGui::Begin("Render stats");
		ImGui::Text("Frame rate: %.1f FPS", ImGui::GetIO().Framerate);
		ImGui::End();
	}

	KyBao::EditorCamera m_Camera;
	KyBao::Ref<KyBao::ShaderLibrary> m_ShaderLibrary;
	KyBao::Ref<KyBao::Object> obj;
	KyBao::TestModel m;
};
#endif
class Sandbox :public KyBao::Application
{
public:
	Sandbox() :Application("Sandbox") {
		//PushLayer(new ExampleLayer());
		PushLayer(new ExampleLayer());
	};
	~Sandbox() {};

private:

};

KyBao::Application* KyBao::CreateApplication() {
	return new Sandbox();
}