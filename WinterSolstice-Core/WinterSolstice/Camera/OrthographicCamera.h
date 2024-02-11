#pragma once 
#include <glm/glm.hpp>

namespace WinterSolstice {
	namespace Rossweisse {

		class OrthographicCamera
		{
		public:
			OrthographicCamera(float left, float right, float bottom, float top);

			const glm::vec3& GetPosition() { return m_Position; };
			void SetPosition(const glm::vec3& position) {
				m_Position = position;
				RecaculateViewMatrix();
			};

			const float& GetRotationX() { return m_RotationX; };
			const float& GetRotationY() { return m_RotationY; };
			const float& GetRotationZ() { return m_RotationZ; };
			void SetRotationX(float rotation) {
				m_RotationX = rotation;
				RecaculateViewMatrix();
			};
			void SetRotationY(float rotation) {
				m_RotationY = rotation;
				RecaculateViewMatrix();
			};
			void SetRotationZ(float rotation) {
				m_RotationZ = rotation;
				RecaculateViewMatrix();
			};

			void SetProjection(float left, float right, float bottom, float top);

			const glm::mat4& GetProjectMatrix() const { return m_ProjectionMatrix; };
			const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; };
			const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; };
		private:
			void RecaculateViewMatrix();
		private:
			glm::mat4 m_ProjectionMatrix;
			glm::mat4 m_ViewMatrix;
			glm::mat4 m_ViewProjectionMatrix;

			glm::vec3 m_Position;
			float m_RotationX = 0.0f;
			float m_RotationY = 0.0f;
			float m_RotationZ = 0.0f;
		};
	}
}