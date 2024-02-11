#include <WinterSolstice.h>
#include "EditorLayout.h"
#include <glm/gtc/type_ptr.hpp>
#include "WinterSolstice/Utils/PlatformUtils.h"
namespace WinterSolstice {
	EditorLayout::EditorLayout()
		:Layer("Editor")
		//, m_CameraController(1280.0f / 720.0f)
	{
		m_ShaderLibrary = CreateRef<Bronya::ShaderLibrary>();
		Application::Get().GetWindow().SetVSync(0);
	}

	void EditorLayout::OnAttach()
	{
		m_Texture2D = Bronya::Texture2D::Create("./assets/textures/Checkerboard.png");
		Bronya::FramebufferSpecification fbsf;
		fbsf.Attachments = { Bronya::FramebufferTextureFormat::RGBA8,
			Bronya::FramebufferTextureFormat::RED_INTEGER,
			Bronya::FramebufferTextureFormat::Depth
		};
		fbsf.Width = 1600.0f;
		fbsf.Height = 900.0f;
		m_Framebuffer = Bronya::Framebuffer::Create(fbsf);

		m_ActiveScene = CreateRef<Raiden::Scene>();
		m_ActiveScene->SetDefaultImage(m_Texture2D);

		editorCamera = Rossweisse::EditorCamera(30.0f, 1.778f, 0.5f, 5000.0f);
#if 0
		auto square = m_ActiveScene->CreateEntity("Green Square1");
		square.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f,1.0f,0.0f,1.0f });
		auto square1 = m_ActiveScene->CreateEntity("Green Square2");
		square1.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f,0.0f,1.0f,1.0f });
		auto square2 = m_ActiveScene->CreateEntity("Green Square3");
		square2.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f,1.0f,1.0f,1.0f });
		auto square3 = m_ActiveScene->CreateEntity("Green Square4");
		square3.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f,1.0f,0.0f,1.0f });

		m_SquareEntity = square;

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera");
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondCamera = m_ActiveScene->CreateEntity("Clip-Space Entity");
		auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
		cc.Primary = false;

		class CameraContorller :public ScritableEntity
		{
		public:
			void OnCreate()
			{
				auto& transform = GetComponent<TransformComponent>().Translation;
				//transform.x = (float)(rand() % 10 - 5.0f);
			}
			void OnDestory() {

			}
			void OnUpdate(Timestep ts)
			{
				auto& transform = GetComponent<TransformComponent>().Translation;
				float speed = 5.0f;
				if (Input::IsKeyPressed(Key::Left))
					transform.x += speed * ts;
				else if (Input::IsKeyPressed(Key::Right))
					transform.x -= speed * ts;
				if (Input::IsKeyPressed(Key::Down))
					transform.y += speed * ts;
				else if (Input::IsKeyPressed(Key::Up))
					transform.y -= speed * ts;

			}
		private:
		};
		//m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraContorller>();
		//m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraContorller>();

#endif



		//std::string TvsSrc = R"(

		//)";
		//std::string TfsSrc = R"(

//)";
		std::string msSrc = R"(
#version 450 core
#extension GL_NV_mesh_shader : require

layout(points) in;
layout(triangle, max_vertices = 3) out;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;

uniform mat4 uViewProjection;
uniform mat4 uModel;

out vec2 vTexcoord;

void main() {
    vec4 position =  uViewProjection * vec4(aPosition, 1.0f);

    // 顶点处理逻辑
    vTexcoord = aTexcoord;

    // 几何处理逻辑
    for (int i = 0; i < 3; ++i) {
        gl_Position = position;
        EmitVertex();
    }
}
)";
		m_ShaderLibrary->Add("Tirangle", Bronya::Shader::Create("./assets/shaders/Model_vs.glsl", "./assets/shaders/Model_fs.glsl"));
		//m_ShaderLibrary->Add("Blue", KyBao::Shader::CreateMeshShader("MeshShader", msSrc));
		m_ShaderLibrary->Add("Blue", Bronya::Shader::Create("./assets/shaders/Texture_vs.glsl", "./assets/shaders/Texture_fs.glsl"));
		//obj = KyBao::Loder::Get()->loadOBJ("./assets/Model/nanosuit/nanosuit.obj");
		//obj = KyBao::Loder::Get()->loadOBJ("C:/Users/sa/Desktop/Assest/just-a-girl/source/final_v01.obj");
		m_Hierarchy.SetContext(m_ActiveScene);
	}

	void EditorLayout::OnDetach()
	{
	}

	void EditorLayout::OnUpdate(Kiana::Timestep ts)
	{
		if (Bronya::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
#ifdef MoreAndMore
			Application::Get().GetMainThread()->AddTaskTop([this]() {
				m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
				});
#else
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
#endif
			//m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			editorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
		//if (m_ViewportFocus)
		{
			//m_CameraController.OnUpdate(ts);
		}
		editorCamera.OnUpdate(ts);

		Himeko::WokerInfo<void()> info;
		info.WokerFunction = [this, ts]()
			{
				Bronya::Renderer2D::ResetStats();
				Bronya::RenderCommand::SetClearColor(glm::vec4{ 0.1f, 0.1f, 0.1f, 1.0f });
				Bronya::RenderCommand::Clear();

				//if (m_ActiveScene->rootBone) {
				//	for (auto& ani : m_ActiveScene->rootBone->name_Animation_map) {
				//		m_ActiveScene->rootBone->Update(ts, ani.first);
				//		break;
				//	}
				//	if (m_ActiveScene->rootBone->name_Animation_map.size() == 0) {
				//		m_ActiveScene->rootBone->BaseUpdate();
				//	}
				//	{
				//		auto shader = m_ShaderLibrary->Get("Blue");
				//		shader->SetMat4s("gBones", m_ActiveScene->rootBone->IDCurrentAnimationTransformMap);
				//	}
				//	{
				//		auto shader = m_ShaderLibrary->Get("Tirangle");
				//		shader->SetMat4s("gBones", m_ActiveScene->rootBone->IDCurrentAnimationTransformMap);
				//	}
				//}
				m_ActiveScene->OnUpdateEditor(ts, editorCamera);
				//m_ActiveScene->OnUpdateRuntime(ts);

				auto [mx, my] = ImGui::GetMousePos();
				mx -= m_ViewportBound[0].x;
				my -= m_ViewportBound[0].y;
				glm::vec2 viewportSize = m_ViewportBound[1] - m_ViewportBound[0];
				my = viewportSize.y - my;
				int mouseX = (int)mx;
				int mouseY = (int)my;

				if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
				{
					int redPexile = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
					{

					}
				}
				Bronya::Renderer::Execute();

			};

		info.FunctionInfo = "Editor Layout __Line__";
#ifdef  MoreAndMore
		Application::Get().GetMainThread()->AddTaskTop(info);
#else
		m_Framebuffer->ClearAttachment(1, -1);
		m_Framebuffer->Bind();
		info.WokerFunction();
		m_Framebuffer->Unbind();
#endif //  MoreAndMore

		//bvh.FlushBVH(obj->GetOBJ());
	}

	void EditorLayout::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
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
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();
		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewSceneAs();

				if (ImGui::MenuItem("Open..", "Ctrl+O"))
					OpenSceneAs();

				if (ImGui::MenuItem("Save As..", "Ctrl+Shfit+S"))
					SaveSceneAs();

				if (ImGui::MenuItem("Loade As.."))
					LoadModel();

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		m_Hierarchy.OnImGuiRender();

		ImGui::Begin("Render stats");

		auto stats = Bronya::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::Text("Frame rate: %.1f FPS", ImGui::GetIO().Framerate);

		ImGui::End();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("Render");
		auto viewportOffset = ImGui::GetCursorPos(); // Includes tab bar

		m_ViewportFocus = ImGui::IsWindowFocused();
		m_ViewportHovers = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->SetBlockFocosEvents(!m_ViewportFocus || !m_ViewportHovers);

		uint32_t renderID = m_Framebuffer->GetColorAttachmentRendererID();
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
		ImGui::Image((void*)renderID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0,1 }, ImVec2{ 1,0 });

		auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBound = ImGui::GetWindowPos();

		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;
		ImVec2 maxBound = { minBound.x + windowSize.x,minBound.y + windowSize.y };
		m_ViewportBound[0] = { minBound.x,minBound.y };
		m_ViewportBound[1] = { maxBound.x,maxBound.y };


		Raiden::Entity selectedEntity = m_Hierarchy.SeletedEntity();
		if (selectedEntity)
		{

		}

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void EditorLayout::OnEvent(KnowTreasure::Event& event)
	{
		//m_CameraController.OnEvent(event);
		editorCamera.OnEvent(event);

		KnowTreasure::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KnowTreasure::KeyPressedEvent>(BIND_EVENT_FN(EditorLayout::OnKeyPressed));
	}

	bool EditorLayout::OnKeyPressed(KnowTreasure::KeyPressedEvent& event)
	{
		if (event.GetRepeatCount() > 0)
			return false;
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (event.GetKeyCode())
		{
		case Key::N:
		{
			if (control)
				NewSceneAs();
			break;
		}
		case Key::O:
		{
			if (control)
				OpenSceneAs();
			break;
		}
		case Key::S:
		{
			if (control)
				SaveSceneAs();
			break;
		}
		}
		return true;
	}

	void EditorLayout::NewSceneAs()
	{
		m_ActiveScene = CreateRef<Raiden::Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_Hierarchy.SetContext(m_ActiveScene);
	}

	void EditorLayout::OpenSceneAs()
	{
		std::string str = FileDialogs::OpenFile("KyBao Scene (*.xml)\0*.xml\0");
		if (!str.empty())
		{
			m_ActiveScene = CreateRef<Raiden::Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_Hierarchy.SetContext(m_ActiveScene);
			Raiden::SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(str);
		}
	}

	void EditorLayout::SaveSceneAs()
	{
		std::string str = FileDialogs::SaveFile("KyBao Scene");
		if (!str.empty())
		{
			Raiden::SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(str);
		}
	}

	void EditorLayout::ObjMapList(Ref<Object>& objs, Raiden::Entity& parent) {
		//设置父节点obj
		parent.AddComponent<Raiden::ObjectComponent>(objs);
		//创建父节点的子节点,为添加子节点做准备
		auto& now = parent.AddComponent<Raiden::NodeComponent>();

		//在list树中获取父节点的父节点 设置当前在list树中的位置
		auto pair = m_ActiveScene->ListAt(parent.Get());
		//获取当前节点在list树中的位置
		now.current = pair.second->child[pair.first];

		for (auto& elemnt : *objs)
		{
			//添加子节点
			auto child = parent.AppendChild(elemnt->GetName());
			//设置子节点值
			child.AddComponent<Raiden::ObjectComponent>(elemnt);
			if (elemnt->GetObjectClass() == ObjectClass::Mesh)
			{
				//如果子节点是mesh那么就不需要再添加子节点了
				auto& childnow = child.AddComponent<Raiden::NodeComponent>();
				auto childpair = m_ActiveScene->ListAt(child.Get());
				//获取当前节点在list树中的位置
				childnow.current = childpair.second->child[childpair.first];
				//设置子节点的材质
				auto ShaderLibrary = CreateRef<Bronya::ShaderLibrary>();
				ShaderLibrary->Add("BaseMaterial" + elemnt->GetName(), Bronya::Shader::Create("./assets/shaders/Texture_vs.glsl", "./assets/shaders/Texture_fs.glsl"));
				child.AddComponent<Raiden::MaterialComponent>(ShaderLibrary);
				auto texs = child.AddComponent<Raiden::SpriteRendererComponent>();
				DrawCall* drawCall = dynamic_cast<DrawCall*>(elemnt.get());
				for (auto tex : drawCall->getTexture())
				{
					texs.Textures[tex.first] = tex.second;
				}
			}
			else if (elemnt->GetObjectClass() == ObjectClass::Model) {
				//当前节点还是model，还需要添加节点
				ObjMapList(elemnt, child);
			}
		}
	}
	void EditorLayout::LoadModel()
	{
		std::string str = FileDialogs::OpenFile("KyBao Scene  (*.*)\0*.*\0");
		if (!str.empty())
		{
			//obj = KyBao::Loder::Get()->loadOBJ("C:/Users/sa/Desktop/Assest/untitled.obj");
			auto objs = loder.loadOBJ(str);
			//添加根节点
			Raiden::Entity parent = m_ActiveScene->CreateEntity(objs->GetName());
			ObjMapList(objs, parent);
			m_ActiveScene->rootBone = loder.bone;
		}
	}

}