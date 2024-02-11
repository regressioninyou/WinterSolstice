#pragma once 
#include "Panels/SceneHierarchyPanel.h"
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
		void NewSceneAs();
		void OpenSceneAs();
		void SaveSceneAs();
		void LoadModel();
		void ObjMapList(Ref<Object>& objs, Raiden::Entity& parent);
	private:
		//OrthographicCameraController m_CameraController;
		Ref<Bronya::VertexArray> m_SquareVA;
		Ref<Bronya::Framebuffer> m_Framebuffer;

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

		bool m_PrimaryCamera = false;

		bool m_ViewportFocus = false;
		bool m_ViewportHovers = false;
		glm::vec2 m_ViewportSize = { 0.0f,0.0f };
		glm::vec2 m_ViewportBound[2];
		glm::vec4 m_SquareColor = { 0.0f,0.1f,0.2f,1.0f };
	};
}