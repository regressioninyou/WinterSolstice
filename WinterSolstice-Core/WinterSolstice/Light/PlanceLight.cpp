#include "PlanceLight.h"
#include "k423.h"
#include "PlanceLight.h"
#include <glm/glm.hpp>
#include "WinterSolstice/Render/Framebuffer.h"
namespace WinterSolstice {
	namespace Kallen {
		PlanceLight::PlanceLight()
		{
		}
		PlanceLight::PlanceLight(glm::vec3 Position)
		{
		}
		PlanceLight::PlanceLight(glm::vec3 Position, glm::vec3 size)
		{
		}
		void PlanceLight::ResizeShadowMap(int width, int height)
		{

		}
		void PlanceLight::DpethBind()
		{
		}
		void PlanceLight::DpethUnBind()
		{
		}
		void PlanceLight::SetPosition(glm::vec3 position) 
		{
		}
		glm::vec3 PlanceLight::GetPosition() 
		{
			return glm::vec3();
		}
		glm::vec3& PlanceLight::GetUsePosition() 
		{
			// TODO: 在此处插入 return 语句
		}
		void PlanceLight::SetScale(glm::vec3 scale) 
		{
		}
		glm::vec3 PlanceLight::GetScale()
		{
			return glm::vec3();
		}
		glm::vec3& PlanceLight::GetUseScale() 
		{
			// TODO: 在此处插入 return 语句
		}
		void PlanceLight::SetRotate(glm::vec3 rotate)
		{
		}
		glm::vec3 PlanceLight::GetRotate()
		{
			return glm::vec3();
		}
		glm::vec3& PlanceLight::GetUseRotate()
		{
			// TODO: 在此处插入 return 语句
		}
		void PlanceLight::LookAt(glm::vec3 lookAt)
		{
		}
		uint32_t PlanceLight::GetTextureID()
		{
			return 0;
		}
		void PlanceLight::BindTexture(uint32_t slot )
		{
			return 0;
		}
		void PlanceLight::UnBindTexture(uint32_t slot )
		{
			return 0;
		}
		void PlanceLight::SetName(const std::string& name)
		{
		}
		std::string PlanceLight::GetName()
		{
			return std::string();
		}
		void PlanceLight::SetRadianceSpotLight(float r)
		{
		}
		void Kallen::PlanceLight::Init()
		{
		}
		void Kallen::PlanceLight::SetIntensity(uint32_t Intensity)
		{
		}
		uint32_t Kallen::PlanceLight::GetIntensity()
		{
			return 0;
		}
		void Kallen::PlanceLight::UsingShaderSetUniform(Ref<Bronya::Shader> shader)
		{
		}
		Ref<Object> Kallen::PlanceLight::GetLightMesh()
		{
			return Ref<Object>();
		}
		Ref<Bronya::Texture2D> Kallen::PlanceLight::GetResultTexture()
		{
			return Ref<Bronya::Texture2D>();
		}
	}
}