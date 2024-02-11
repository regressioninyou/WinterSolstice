#pragma once 
#include <WinterSolstice/Core/Timestep.h>
#include "OrthographicCamera.h"
#include <WinterSolstice/Events/MouseEvent.h>
#include <WinterSolstice/Events/ApplicationEvent.h>

namespace WinterSolstice {
	namespace Rossweisse {
		class OrthographicCameraController
		{
		public:
			OrthographicCameraController(float aspectRatio, bool rotation = false);

			void OnUpdate(Kiana::Timestep ts);
			void OnEvent(KnowTreasure::Event& e);

			void OnResize(float width, float height);

			OrthographicCamera& GetCamera() { return m_Camera; }
			const OrthographicCamera& GetCamera() const { return m_Camera; }

			float GetZoomLevel() const { return m_ZoomLevel; }
			void SetZoomLevel(float level) { m_ZoomLevel = level; }
		private:
			bool OnMouseScrolled(KnowTreasure::MouseScrolledEvent& e);
			bool OnWindowResized(KnowTreasure::WindowResizeEvent& e);
			bool OnMouseMoved(KnowTreasure::MouseMovedEvent& e);
			bool OnButtonPressed(KnowTreasure::MouseButtonPressedEvent& e);
			bool OnButtonReleased(KnowTreasure::MouseButtonReleasedEvent& e);
		private:
			float m_AspectRatio;
			float m_ZoomLevel = 10.0f;
			OrthographicCamera m_Camera;
			bool click = false;
			bool m_Rotation;
			glm::vec2 XY = { 0.0f,0.0f };

			glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
			float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
			float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
		};
	}
}