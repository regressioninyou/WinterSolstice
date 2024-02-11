#include <k423.h>
#include "Light.h"
#include "WinterSolstice/Light/PointLight.h"
namespace WinterSolstice {
	namespace Kallen {
		Ref<Light> Light::CreatePointLight()
		{
			return CreateRef<PointLight>();
		}
		Ref<Light> Light::CreatePointLight(glm::vec3 Position)
		{
			return CreateRef<PointLight>(Position);
		}
		Ref<Light> Light::CreatePlanceLight()
		{
			return Ref<Light>();
		}
		Ref<Light> Light::CreatePlanceLight(glm::vec3 Position)
		{
			return Ref<Light>();
		}
		Ref<Light> Light::CreatePlanceLight(glm::vec3 Position, glm::vec3 size)
		{
			return Ref<Light>();
		}
	}
}