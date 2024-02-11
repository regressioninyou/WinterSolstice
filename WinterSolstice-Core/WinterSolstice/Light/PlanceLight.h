#pragma once 
#include "Light.h"
namespace WinterSolstice {
	namespace Bronya {
		class Framebuffer;
	}
	namespace Kallen {
		class PlanceLight : public Light
		{
		public:
			PlanceLight();
			PlanceLight(glm::vec3 Position);
			PlanceLight(glm::vec3 Position, glm::vec3 size);
			
			virtual void ResizeShadowMap(int width, int height) override;
			virtual void DpethBind() override;
			virtual void DpethUnBind() override;
			virtual void SetPosition(glm::vec3 position)  override;
			virtual glm::vec3 GetPosition()  override;
			virtual glm::vec3& GetUsePosition()  override;
			virtual void SetScale(glm::vec3 scale)  override;
			virtual glm::vec3 GetScale() override;
			virtual glm::vec3& GetUseScale()  override;
			virtual void SetRotate(glm::vec3 rotate) override;
			virtual glm::vec3 GetRotate() override;
			virtual glm::vec3& GetUseRotate() override;
			virtual void LookAt(glm::vec3 lookAt) override;
			virtual uint32_t GetTextureID() override;
			virtual void BindTexture(uint32_t slot = 0) override;
			virtual void UnBindTexture(uint32_t slot = 0) override;
			virtual void SetName(const std::string& name) override;
			virtual std::string GetName() override;
			virtual void SetRadianceSpotLight(float r) override;
			// 通过 Light 继承
			void Init() override;

			// 通过 Light 继承
			void SetIntensity(uint32_t Intensity) override;
			uint32_t GetIntensity() override;
			void UsingShaderSetUniform(Ref<Bronya::Shader> shader) override;
			Ref<Object> GetLightMesh() override;
		private:
			Ref<Bronya::Framebuffer> shadowMap;


			// 通过 Light 继承
			Ref<Bronya::Texture2D> GetResultTexture() override;

		};
	}
}