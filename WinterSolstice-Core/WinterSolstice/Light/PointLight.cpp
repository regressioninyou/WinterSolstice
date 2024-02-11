#include "PointLight.h"
#include "k423.h"
#include "PointLight.h"
#include <glm/glm.hpp>
#include "WinterSolstice/Render/Framebuffer.h"
namespace WinterSolstice {
	namespace Kallen {
		PointLight::PointLight(glm::vec3 Position, glm::vec3 Color, float Intensity, float Range)
			:m_Position(Position), m_Color(Color), m_Intensity(Intensity), m_Range(Range)
		{
			LookAt(glm::vec3(0.0f));
		}
		LightType PointLight::GetLightType() const
		{
			return LightType::Point;
		}
		glm::vec3 PointLight::GetPosition() const
		{
			return m_Position;
		}
		glm::vec3 PointLight::GetDirection() const
		{
			return m_Direction;
		}
		glm::vec3 PointLight::GetColor() const
		{
			return m_Color;
		}
		float PointLight::GetIntensity() const
		{
			return m_Intensity;
		}
		float PointLight::GetRange() const
		{
			return m_Range;
		}
		float PointLight::GetSpotAngle() const
		{
			return 0.0f;
		}
		float PointLight::GetSpotExponent() const
		{
			return 0.0f;
		}
		void PointLight::SetPosition(const glm::vec3& position)
		{
			m_Position = position;
		}
		void PointLight::SetDirection(const glm::vec3& direction)
		{
			m_Direction = direction;
		}
		void PointLight::SetColor(const glm::vec3& color)
		{
			m_Color = color;
		}
		void PointLight::SetIntensity(float intensity)
		{
			m_Intensity = intensity;
		}
		void PointLight::SetRange(float range)
		{
			m_Range = range;	
		}
		float PointLight::GetFocalPoint() const
		{
			return m_Direction;
		}
		void PointLight::LookAt(const glm::vec3& point)
		{
			m_Direction = glm::normalize(point - m_Position);
		}
	}
}