#include "k423.h"
#include "WinterSolstice/Camera/OrthographicCameraController.h"
#include "WinterSolstice/Core/Input.h"
#include "WinterSolstice/Core/KeyCodes.h"
#include <WinterSolstice/Core/Application.h>
namespace WinterSolstice {
	namespace Rossweisse {
		OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
			:m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
		{

		}
		void OrthographicCameraController::OnUpdate(Kiana::Timestep ts)
		{
			if (Input::IsKeyPressed(Key::A))
			{
				m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
				m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			}
			else if (Input::IsKeyPressed(Key::D))
			{
				m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
				m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			}

			if (Input::IsKeyPressed(Key::W))
			{
				m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
				m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			}
			else if (Input::IsKeyPressed(Key::S))
			{
				m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
				m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			}
			else if (Input::IsKeyPressed(Key::Space))
			{
				m_CameraPosition.z += m_CameraTranslationSpeed * ts;
			}
			else if (Input::IsKeyPressed(Key::LeftControl))
			{
				m_CameraPosition.z -= m_CameraTranslationSpeed * ts;
			}
			if (m_Rotation)
			{
				if (Input::IsKeyPressed(Key::Q))
					m_Rotation -= m_CameraRotationSpeed;
				else if (Input::IsKeyPressed(Key::E))
					m_Rotation += m_CameraRotationSpeed;


				if (m_CameraRotation > 180.0f)
					m_CameraRotation -= 360.0f;
				else if (m_CameraRotation <= -180.0f)
					m_CameraRotation += 360.0f;

				m_Camera.SetRotationZ(m_Rotation);
			}
			m_Camera.SetPosition(m_CameraPosition);

			m_CameraTranslationSpeed = m_ZoomLevel;
		}
		void OrthographicCameraController::OnEvent(KnowTreasure::Event& e)
		{
			KnowTreasure::EventDispatcher dispatcher(e);
			dispatcher.Dispatch<KnowTreasure::MouseScrolledEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
			dispatcher.Dispatch<KnowTreasure::WindowResizeEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
			dispatcher.Dispatch<KnowTreasure::MouseMovedEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseMoved));
			dispatcher.Dispatch<KnowTreasure::MouseButtonPressedEvent>(BIND_EVENT_FN(OrthographicCameraController::OnButtonPressed));
			dispatcher.Dispatch<KnowTreasure::MouseButtonReleasedEvent>(BIND_EVENT_FN(OrthographicCameraController::OnButtonReleased));
		}
		void OrthographicCameraController::OnResize(float width, float height)
		{
			m_AspectRatio = width / height;
			m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		}
		bool OrthographicCameraController::OnMouseScrolled(KnowTreasure::MouseScrolledEvent& e)
		{
			m_ZoomLevel -= e.GetYOffset() * 0.25f;
			m_ZoomLevel = (((m_ZoomLevel) > (0.25f)) ? (m_ZoomLevel) : (0.25f));

			m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
			return false;
		}
		bool OrthographicCameraController::OnWindowResized(KnowTreasure::WindowResizeEvent& e)
		{
			OnResize((float)e.GetWidth(), (float)e.GetHeight());
			return false;
		}
		bool OrthographicCameraController::OnMouseMoved(KnowTreasure::MouseMovedEvent& e)
		{
			if (click)
			{
				float x = XY.x - e.GetX();
				float y = XY.y - e.GetY();
				m_Camera.SetRotationX(m_Camera.GetRotationX() - x * 0.1);
				m_Camera.SetRotationY(m_Camera.GetRotationY() - y * 0.1);
				XY.x = e.GetX();
				XY.y = e.GetY();
			}
			XY.x = e.GetX();
			XY.y = e.GetY();
			return false;
		}
		bool OrthographicCameraController::OnButtonPressed(KnowTreasure::MouseButtonPressedEvent& e)
		{
			click = true;
			return false;
		}
		bool OrthographicCameraController::OnButtonReleased(KnowTreasure::MouseButtonReleasedEvent& e)
		{
			click = false;
			return false;
		}
	}
}