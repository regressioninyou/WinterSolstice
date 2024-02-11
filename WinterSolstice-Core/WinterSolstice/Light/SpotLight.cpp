#include "SpotLight.h"
#include "k423.h"
#include "SpotLight.h"
#include <glm/glm.hpp>
#include "WinterSolstice/Render/Framebuffer.h"
namespace WinterSolstice {
	namespace Kallen {
		SpotLight::SpotLight()
		{
		}
		void SpotLight::ResizeShadowMap(int width, int height)
		{
		}
		void SpotLight::DpethBind()
		{
		}
		void SpotLight::DpethUnBind()
		{
		}
		void SpotLight::SetPosition(glm::vec3 position) 
		{
		}
		glm::vec3 SpotLight::GetPosition() 
		{
			return glm::vec3();
		}
		glm::vec3& SpotLight::GetUsePosition() 
		{
			// TODO: 在此处插入 return 语句
		}
		void SpotLight::SetScale(glm::vec3 scale) 
		{
		}
		glm::vec3 SpotLight::GetScale()
		{
			return glm::vec3();
		}
		glm::vec3& SpotLight::GetUseScale() 
		{
			return glm::vec3();
		}
		void SpotLight::SetRotate(glm::vec3 rotate)
		{
		}
		glm::vec3 SpotLight::GetRotate()
		{
			return glm::vec3();
		}
		glm::vec3& SpotLight::GetUseRotate()
		{
			// TODO: 在此处插入 return 语句
		}
		void SpotLight::LookAt(glm::vec3 lookAt)
		{
		}
		uint32_t SpotLight::GetTextureID()
		{
			return 0;
		}
		void SpotLight::BindTexture(uint32_t slot )
		{
		}
		void SpotLight::UnBindTexture(uint32_t slot )
		{
		}
		void SpotLight::SetName(const std::string& name)
		{
		}
		std::string SpotLight::GetName()
		{
			return std::string();
		}
		void SpotLight::SetRadianceSpotLight(float r)
		{
		}
		void Kallen::SpotLight::Init()
		{
		}
		void Kallen::SpotLight::SetIntensity(uint32_t Intensity)
		{
		}
		uint32_t Kallen::SpotLight::GetIntensity()
		{
			return 0;
		}
		void Kallen::SpotLight::UsingShaderSetUniform(Ref<Bronya::Shader> shader)
		{
		}
		Ref<Object> Kallen::SpotLight::GetLightMesh()
		{
			return Ref<Object>();
		}
		Ref<Bronya::Texture2D> Kallen::SpotLight::GetResultTexture()
		{
			return Ref<Bronya::Texture2D>();
		}
	}
}