#pragma once 
#include <glm/glm.hpp>
#include "WinterSolstice/Render/Texture.h"
namespace WinterSolstice {
	namespace  Bronya {
		class Shader;
	}
	class Object;
	namespace Kallen {
		enum class LightType {
			Point,
			Directional,
			Spot
		};
		class Light
		{
		public:
			virtual ~Light() = default;
			virtual LightType GetLightType() const = 0;
			virtual glm::vec3 GetPosition() const = 0;
			virtual glm::vec3 GetDirection() const = 0;
			virtual glm::vec3 GetColor() const = 0;
			virtual float GetIntensity() const = 0;
			virtual float GetRange() const = 0;
			virtual float GetSpotAngle() const = 0;
			virtual float GetSpotExponent() const = 0;
			virtual glm::vec3 GetFocalPoint() const = 0;
			
			virtual void SetPosition(const glm::vec3& position) = 0;
			virtual void SetDirection(const glm::vec3& direction) = 0;
			virtual void SetColor(const glm::vec3& color) = 0;
			virtual void SetIntensity(float intensity) = 0;
			virtual void SetRange(float range) = 0;
			virtual void LookAt(const glm::vec3& point) = 0;

			static Ref<Light> CreatePlanceLight();
			static Ref<Light> CreatePlanceLight(glm::vec3 Position);
			static Ref<Light> CreatePlanceLight(glm::vec3 Position, glm::vec3 size);

			static Ref<Light> CreatePointLight();
			static Ref<Light> CreatePointLight(glm::vec3 Position);
		};
	}
}