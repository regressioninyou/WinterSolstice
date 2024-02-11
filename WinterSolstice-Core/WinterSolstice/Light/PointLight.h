#pragma once 
#include "Light.h"
namespace WinterSolstice {
	namespace Bronya {
		class Framebuffer;
	}
	namespace Kallen {
		class PointLight : public Light
		{
		public:
			PointLight(glm::vec3 Position = glm::vec3(0.0f, 10.0f, 0.0f),
				glm::vec3 Color = glm::vec3(1.0f),
				float Intensity = 5000.0f,
				float Range = 5000.0f);

			// 通过 Light 继承
			LightType GetLightType() const override;

			glm::vec3 GetPosition() const override;

			glm::vec3 GetDirection() const override;

			glm::vec3 GetColor() const override;

			float GetIntensity() const override;

			float GetRange() const override;

			float GetSpotAngle() const override;

			float GetSpotExponent() const override;

			// 通过 Light 继承
			glm::vec3 GetFocalPoint() const override;

			void SetPosition(const glm::vec3& position) override;

			void SetDirection(const glm::vec3& direction) override;

			void SetColor(const glm::vec3& color) override;

			void SetIntensity(float intensity) override;

			void SetRange(float range) override;

			void LookAt(const glm::vec3& point) override;
		private:
			glm::vec3 m_Position;
			glm::vec3 m_Color;
			glm::vec3 m_Direction;
			float m_Intensity;
			float m_Range;
		};
	}
}