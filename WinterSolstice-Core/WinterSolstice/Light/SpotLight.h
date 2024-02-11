#pragma once 
#include "Light.h"
namespace WinterSolstice {
	namespace Bronya {
		class Framebuffer;
	}
	namespace Kallen {
		class SpotLight : public Light
		{
		public:
			SpotLight();

			void ResizeShadowMap(int width, int height) override;

			void DpethBind() override;
			void DpethUnBind() override;

			void SetPosition(glm::vec3 position)  override;
			glm::vec3 GetPosition()  override;
			glm::vec3& GetUsePosition()  override;

			void SetScale(glm::vec3 scale) override;
			glm::vec3 GetScale() override;
			glm::vec3& GetUseScale()  override;

			void SetRotate(glm::vec3 rotate) override;
			glm::vec3 GetRotate() override;
			glm::vec3& GetUseRotate() override;

			void LookAt(glm::vec3 lookAt) override;

			uint32_t GetTextureID() override;
			void BindTexture(uint32_t slot = 0) override;
			void UnBindTexture(uint32_t slot = 0) override;

			void SetName(const std::string& name) override;
			std::string GetName() override;
			void SetRadianceSpotLight(float r) override;

		private:
			Ref<Bronya::Framebuffer> shadowMap;

			// 通过 Light 继承
			void Init() override;

			// 通过 Light 继承
			void SetIntensity(uint32_t Intensity) override;
			uint32_t GetIntensity() override;
			void UsingShaderSetUniform(Ref<Bronya::Shader> shader) override;
			Ref<Object> GetLightMesh() override;

			// 通过 Light 继承
			Ref<Bronya::Texture2D> GetResultTexture() override;
		};
	}
}