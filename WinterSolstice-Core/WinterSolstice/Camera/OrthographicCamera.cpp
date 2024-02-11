#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace WinterSolstice {
	namespace Rossweisse {
		OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
			:m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -10.0f, 1000.0f)), m_ViewMatrix(1.0f), m_Position(0.0f)
		{
			m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
		}
		void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
		{
			m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
			m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
		}
		void OrthographicCamera::RecaculateViewMatrix()
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)
				* glm::rotate(glm::mat4(1.0f), glm::radians(m_RotationX), glm::vec3(1.0f, 0.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), glm::radians(m_RotationY), glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), glm::radians(m_RotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
			//m_ViewMatrix = glm::inverse(transform);
			m_ViewProjectionMatrix = m_ProjectionMatrix * transform;
		}
	}
}