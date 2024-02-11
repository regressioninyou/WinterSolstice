#include "k423.h"
#include "SceneCamera.h"
#include <glm/ext/matrix_clip_space.hpp>

namespace WinterSolstice {
	namespace Rossweisse {
		SceneCamera::SceneCamera()
			:Camera(glm::mat4(1.0f))
		{
			RecalculateProjection();
		}
		void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
		{
			matType = ProjectionType::OrthoGraphics;
			m_OrthographicFar = farClip;
			m_OrthographicSize = size;
			m_OrthographicNear = nearClip;
			RecalculateProjection();
		}
		void SceneCamera::SetPerspective(float fov, float nearClip, float farClip)
		{
			matType = ProjectionType::Perspective;
			m_PerspectiveFOV = fov;
			m_PerspectiveNear = nearClip;
			m_PerspectiveFar = farClip;
			RecalculateProjection();
		}
		void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
		{
			m_AspectRatio = (float)width / (float)height;
			RecalculateProjection();
		}

		void SceneCamera::RecalculateProjection() {
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(font.x), { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), glm::radians(font.y), { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), glm::radians(font.z), { 0, 0, 1 });
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * rotation;
			switch (matType)
			{
			case SceneCamera::ProjectionType::Perspective:
				m_Projection = glm::perspective(glm::radians(m_PerspectiveFOV), m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
				break;
			case SceneCamera::ProjectionType::OrthoGraphics:
			{
				float orthoLeft = 0.5 * m_OrthographicSize * -m_AspectRatio;
				float orthoRight = 0.5 * m_OrthographicSize * m_AspectRatio;
				float orthoTop = (0.5 * m_OrthographicSize);
				float orthoBottom = -(0.5 * m_OrthographicSize);
				m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
			}break;
			}

		}
	}
}