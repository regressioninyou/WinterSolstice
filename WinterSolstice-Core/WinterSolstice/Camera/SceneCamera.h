#pragma once 
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace WinterSolstice {
	namespace Rossweisse{
		class SceneCamera :public Camera
		{
		public:
			enum class ProjectionType { Perspective = 0, OrthoGraphics = 1 };
		public:
			SceneCamera();
			SceneCamera(glm::mat4 proj) :Camera(proj) {};
			virtual ~SceneCamera() = default;

			void SetOrthographic(float size, float nearClip, float farClip);
			void SetPerspective(float fov, float nearClip, float farClip);
			void SetViewportSize(uint32_t width, uint32_t height);


			float GetOrthographicsSize()const { return m_OrthographicSize; };
			float GetOrthographicsFar()const { return m_OrthographicFar; }
			float GetOrthographicsNear()const { return m_OrthographicNear; };

			void SetOrthographicsSize(float size) { m_OrthographicSize = size; RecalculateProjection(); };
			void SetOrthographicsFar(float FarClip) { m_OrthographicFar = FarClip; RecalculateProjection(); }
			void SetOrthographicsNear(float NearClip) { m_OrthographicNear = NearClip; RecalculateProjection(); };

			float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
			float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
			float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
			void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjection(); }
			void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
			void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }

			glm::vec3 GetPosition() { return m_Position; }
			void SetPosition(glm::vec3 pos) { m_Position = pos; }

			ProjectionType GetProjectType() { return matType; }
			void SetProjectType(ProjectionType type) { matType = type; RecalculateProjection(); };
			void SetFont(glm::vec3 f) {
				font = f;
			}
			glm::vec3 GetFont() { return font; }
			glm::vec3 GetFont(glm::vec3 Rotation) {
				glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), { 1, 0, 0 })
					* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), { 0, 1, 0 })
					* glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), { 0, 0, 1 });
				return glm::vec4(font, 1.0f) * rotation;
			};
		private:
			void RecalculateProjection();

			ProjectionType matType = ProjectionType::OrthoGraphics;
			float m_OrthographicSize = 40.0f;
			float m_OrthographicNear = -1.0f;
			float m_OrthographicFar = 1000.0f;

			float m_PerspectiveFOV = 40.0f;
			float m_PerspectiveNear = 0.01f;
			float m_PerspectiveFar = 1000.0f;

			glm::vec3 m_Position = { 0.0f,0.0f,0.0f };

			glm::vec3 font = { 0.0f,-1.0f,0.0f };

			float m_AspectRatio = 0.0f;
		};
	}
}