#include <WinterSolstice.h>
#include "EditorLayout.h"
#include <glm/gtc/type_ptr.hpp>
#include "WinterSolstice/Utils/PlatformUtils.h"
#define SHADERSOURCE(x) std::string("T:/orther/main/c++/WinterSolstice/WinterSolstice-Editory/assets/shaders/").append(x)
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
		fbsf.Attachments = {
			Bronya::FramebufferTextureFormat::RED_INTEGER,
			Bronya::FramebufferTextureFormat::RGBA16F,
			Bronya::FramebufferTextureFormat::RGBA16F,
			Bronya::FramebufferTextureFormat::RGBA16F,
			Bronya::FramebufferTextureFormat::Depth
		};
		fbsf.Width = 1600.0f;
		fbsf.Height = 900.0f;
		m_Framebuffer = Bronya::Framebuffer::Create(fbsf);

		Bronya::FramebufferSpecification gBuffer;
		gBuffer.Attachments = {
			Bronya::FramebufferTextureFormat::RGBA16F,
			Bronya::FramebufferTextureFormat::RED_INTEGER,
			Bronya::FramebufferTextureFormat::Depth
		};
		gBuffer.Width = 1600.0f;
		gBuffer.Height = 900.0f;

		m_Gbuffer = Bronya::Framebuffer::Create(gBuffer);

		m_ActiveScene = CreateRef<Raiden::Scene>();
		m_ActiveScene->SetDefaultImage(m_Texture2D);

		editorCamera = Rossweisse::EditorCamera(30.0f, 1.778f, 0.5f, 5000.0f);

		m_ShaderLibrary->Add("Tirangle", Bronya::Shader::Create("./assets/shaders/Model_vs.glsl", "./assets/shaders/Model_fs.glsl"));
		//m_ShaderLibrary->Add("Blue", KyBao::Shader::CreateMeshShader("MeshShader", msSrc));
		m_ShaderLibrary->Add("Blue", Bronya::Shader::Create("./assets/shaders/Texture_vs.glsl", "./assets/shaders/Texture_fs.glsl"));
		//obj = KyBao::Loder::Get()->loadOBJ("./assets/Model/nanosuit/nanosuit.obj");
		//obj = KyBao::Loder::Get()->loadOBJ("C:/Users/sa/Desktop/Assest/just-a-girl/source/final_v01.obj");
		m_Hierarchy.SetContext(m_ActiveScene);

		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		uint32_t offset = 0;

		uint32_t quadIndices[] = { 0, 1, 2,1, 3, 2 };
		m_SquareVA = Bronya::VertexArray::Create();
		auto vabuffer = Bronya::VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		{
			Bronya::BufferLayout layer = {
				{ Bronya::ShaderDataType::Float3,"aPosition"},
				{ Bronya::ShaderDataType::Float2,"aTexCoord"}
			};
			vabuffer->SetLayout(layer);
		}
		auto idsbuffer = Bronya::IndexBuffer::Create(quadIndices, 6);
		m_SquareVA->AddVertexBuffer(vabuffer);
		m_SquareVA->SetIndexBuffer(idsbuffer);
		m_SquareVAShader = Bronya::Shader::Create(SHADERSOURCE("GBuffer/deferred.vs"), SHADERSOURCE("GBuffer/deferred.fs"));
		const unsigned int NR_LIGHTS = 32;
		for (unsigned int i = 0; i < NR_LIGHTS; i++)
		{
			// calculate slightly random offsets
			float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
			float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
			float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
			lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
			// also calculate random color
			float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.)
			float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.)
			float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.)
			lightColors.push_back(glm::vec3(rColor, gColor, bColor));
		}
	}

	void EditorLayout::OnDetach()
	{
	}
#define USE_IMGUI_GBUFFER true
	void EditorLayout::OnUpdate(Kiana::Timestep ts)
	{
		if (Bronya::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			Application::Get().RenderThread()->AddTaskAwait([this]()
				{
					m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
					m_Gbuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
				});
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
				m_Framebuffer->Bind();
				Bronya::RenderCommand::SetClearColor(glm::vec4{ 0.1f, 0.1f, 0.1f, 1.0f });
				Bronya::RenderCommand::Clear();
				m_Framebuffer->ClearAttachment(0, -1);

				m_ActiveScene->OnUpdateEditor(ts, editorCamera);
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
				//m_ActiveScene->OnUpdateRuntime(ts);
				Bronya::Renderer::Flush();
				Bronya::Renderer2D::Flush();
				m_Framebuffer->Unbind();
			};

		info.FunctionInfo = "Editor Layout ";
#if USE_IMGUI_GBUFFER
		Application::Get().RenderThread()->AddTaskAwaitInfo(info);
#endif
		auto task = [this]()->bool
			{
				m_Framebuffer->BindColorAttachments();
				for (unsigned int i = 0; i < lightPositions.size(); i++)
				{
					m_SquareVAShader->SetFloat3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
					m_SquareVAShader->SetFloat3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
					// update attenuation parameters and calculate radius
					const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
					const float linear = 0.7f;
					const float quadratic = 1.8f;
					m_SquareVAShader->SetFloat("lights[" + std::to_string(i) + "].Linear", linear);
					m_SquareVAShader->SetFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
					// then calculate radius of light volume/sphere
					const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
					float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
					m_SquareVAShader->SetFloat("lights[" + std::to_string(i) + "].Radius", radius * 10.0f);
				}
				m_SquareVAShader->SetInt("viewWidth", m_ViewportSize.x);
				m_SquareVAShader->SetInt("viewHeight", m_ViewportSize.y);
				m_SquareVAShader->SetFloat3("viewPos", editorCamera.GetPosition());
				return true;
			};

#if USE_IMGUI_GBUFFER
		AwaitSwapFramerBufferAndInitScene(m_Framebuffer, m_Gbuffer, task);
#endif
		auto SubScreen = [this, info]()
			{
				m_Gbuffer->Bind();
#if !USE_IMGUI_GBUFFER
				Bronya::RenderCommand::SetClearColor(glm::vec4{ 0.1f, 0.1f, 0.1f, 1.0f });
				Bronya::RenderCommand::Clear();
				m_Gbuffer->ClearAttachment(1, -1);
#endif
				Bronya::Renderer2D::ResetStats();
				Bronya::Renderer2D::BeginScene(editorCamera);
				glm::mat4 model = glm::mat4(1.0f);
				for (unsigned int i = 0; i < lightPositions.size(); i++)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, lightPositions[i]);
					model = glm::scale(model, glm::vec3(0.125f));
					Bronya::Renderer2D::DrawCircle(model, glm::vec4(lightColors[i], 1.0f), 1.0f, 0.005f, i + 1);
				}
				Bronya::Renderer2D::EndScene();
				Bronya::Renderer2D::Flush();

				m_Gbuffer->Unbind();
			};
		Application::Get().RenderThread()->AddTaskAwait(SubScreen);
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
		{
			ImGui::Begin("Render");
			auto viewportOffset = ImGui::GetCursorPos(); // Includes tab bar

			m_ViewportFocus = ImGui::IsWindowFocused();
			m_ViewportHovers = ImGui::IsWindowHovered();
			Application::Get().GetImGuiLayer()->SetBlockFocosEvents(!m_ViewportHovers);

			uint32_t renderID = m_Gbuffer->GetColorAttachmentRendererID();
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

			m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
			ImGui::Image((void*)renderID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0,1 }, ImVec2{ 1,0 });

			auto windowSize = ImGui::GetWindowSize();
			ImVec2 minBound = ImGui::GetWindowPos();
			WindowSize.x = windowSize.x;
			WindowSize.y = windowSize.y;
			WindowPos.x = minBound.x;
			WindowPos.y = minBound.y;

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
		}
#if USE_IMGUI_GBUFFER
		{
			ImGui::Begin("GBufferPosition");

			uint32_t renderID = m_Framebuffer->GetColorAttachmentRendererID(1);
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

			m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
			ImGui::Image((void*)renderID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0,1 }, ImVec2{ 1,0 });


			ImGui::End();
		}
		{
			ImGui::Begin("GBufferNormal");

			uint32_t renderID = m_Framebuffer->GetColorAttachmentRendererID(2);
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

			m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
			ImGui::Image((void*)renderID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0,1 }, ImVec2{ 1,0 });


			ImGui::End();
		}
		{
			ImGui::Begin("GBufferAlbedo");

			uint32_t renderID = m_Framebuffer->GetColorAttachmentRendererID(3);
			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

			m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
			ImGui::Image((void*)renderID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2{ 0,1 }, ImVec2{ 1,0 });


			ImGui::End();
		}
#endif
		ImGui::PopStyleVar();

		ImGui::End();

	}

	void EditorLayout::OnEvent(KnowTreasure::Event& event)
	{
		//m_CameraController.OnEvent(event);
		editorCamera.OnEvent(event);

		KnowTreasure::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KnowTreasure::KeyPressedEvent>(BIND_EVENT_FN(EditorLayout::OnKeyPressed));
		dispatcher.Dispatch<KnowTreasure::MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayout::OnMouseButtonPressed));
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

	bool EditorLayout::OnMouseButtonPressed(KnowTreasure::MouseButtonPressedEvent& event)
	{
		if (event.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovers && !Input::IsKeyPressed(Key::LeftAlt)) {
				Application::Get().TaskThread()->AddTask([this]()
					{
						auto [mx, my] = ImGui::GetMousePos();
						mx -= m_ViewportBound[0].x;
						my -= m_ViewportBound[0].y;
						glm::vec2 viewportSize = m_ViewportBound[1] - m_ViewportBound[0];
						my = viewportSize.y - my;
						int mouseX = (int)mx;
						int mouseY = (int)my;

						if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
						{
							Application::Get().RenderThread()->AddTask([this, mouseY, mouseX]()
								{
									m_Gbuffer->Bind();
									int redPexile = m_Gbuffer->ReadPixel(1, mouseX, mouseY);
									{
										Kiana_CORE_INFO("to ReadPexile {0} , X: {1} ; Y:{2} ", redPexile, mouseX, mouseY);
									}
									m_Gbuffer->Unbind();
									if (redPexile > -1) {
										m_Hierarchy.SetSelectedEntity(Raiden::Entity((entt::entity)redPexile, m_ActiveScene.get()));
									}
								});
						}
					});
			}
		}
		return false;
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
				//ShaderLibrary->Add("BaseMaterial" + elemnt->GetName(),
					//Bronya::Shader::Create("./assets/shaders/Texture_vs.glsl", "./assets/shaders/Texture_fs.glsl"));
				ShaderLibrary->Add("g_BufferMaterial" + elemnt->GetName(),
					Bronya::Shader::Create(SHADERSOURCE("GBuffer/g_Buffer.vs"), SHADERSOURCE("GBuffer/g_Buffer.fs")));
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
	void EditorLayout::AwaitSwapFramerBufferAndInitScene(Ref<Bronya::Framebuffer> from, Ref<Bronya::Framebuffer> to, std::function<bool()> callback)
	{
		Application::Get().RenderThread()->AddTaskAwait([this, from, to, callback]()
			{
				to->BindDrawBuffer(to->GetRendererID());
				Bronya::RenderCommand::SetClearColor(glm::vec4{ 0.1f, 0.1f, 0.1f, 1.0f });
				Bronya::RenderCommand::Clear();
				to->ClearAttachment(1, -1);
				Bronya::Renderer::ResetStats();
				Bronya::Renderer::BeginScene(editorCamera);

				Bronya::Renderer::SubmitQue(m_SquareVA, m_SquareVAShader, std::move(callback), glm::vec3(0.0f), true);
				Bronya::Renderer::EndScene();
				Bronya::Renderer::Flush();
				from->BindReadBuffer(from->GetRendererID());
				from->ToOtherFramebuffer(to->GetRendererID(), to->GetSpecification());
			});
	}
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
}