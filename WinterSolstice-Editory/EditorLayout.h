#pragma once 
#include "Panels/SceneHierarchyPanel.h"
#include "ScreenPostProcessing/ScreenPostProcessing.h"
namespace WinterSolstice {
	class EditorLayout : public Layer
	{
	public:
		EditorLayout();
		virtual ~EditorLayout() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Kiana::Timestep ts)  override;
		virtual void OnImGuiRender()  override;
		virtual void OnEvent(KnowTreasure::Event& event)  override;
	private:
		bool OnKeyPressed(KnowTreasure::KeyPressedEvent& event);
		bool OnMouseButtonPressed(KnowTreasure::MouseButtonPressedEvent& event);
		void NewSceneAs();
		void OpenSceneAs();
		void SaveSceneAs();
		void LoadModel();
		void AwaitSwapFramerBufferAndInitScene(Ref<Bronya::Framebuffer> from,Ref<Bronya::Framebuffer> to,std::function<bool()> callback);
		void ObjMapList(Ref<Object>& objs, Raiden::Entity& parent);
	private:
		//OrthographicCameraController m_CameraController;
		Ref<Bronya::VertexArray> m_SquareVA;
		Ref<Bronya::Shader> m_SquareVAShader;
		//Ref<Bronya::Shader> HDR_shader;

		std::vector<glm::vec3> lightPositions;
		std::vector<glm::vec3> lightColors;

		ImVec2  WindowSize;
		ImVec2  WindowPos;

		Ref<Bronya::Framebuffer> m_Gbuffer;
		Ref<Bronya::Framebuffer> m_Framebuffer;
		//Ref<Bronya::Framebuffer> HDR_Buffer;

		Ref<Bronya::Texture2D> m_Texture2D;

		Ref<Bronya::ShaderLibrary> m_ShaderLibrary;
		BVHAccel bvh;
		Loder loder;

		Rossweisse::EditorCamera editorCamera;

		Ref<Raiden::Scene> m_ActiveScene;
		Raiden::Entity m_SquareEntity;
		Raiden::Entity m_CameraEntity;
		Raiden::Entity m_SecondCamera;

		SceneHierarchyPanel m_Hierarchy;
		ScreenPostProcessing m_ScreenProcess;

		bool m_PrimaryCamera = false;

		bool m_ViewportFocus = false;
		bool m_ViewportHovers = false;
		glm::vec2 m_ViewportSize = { 0.0f,0.0f };
		glm::vec2 m_ViewportBound[2];
		glm::vec4 m_SquareColor = { 0.0f,0.1f,0.2f,1.0f };

		int m_GizmoType = -1;
		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;
	};
}