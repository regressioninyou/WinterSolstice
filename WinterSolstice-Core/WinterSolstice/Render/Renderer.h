#pragma once 
#include "WinterSolstice/Render/RendererCommand.h"
#include <WinterSolstice/Camera/SceneCamera.h>
#include <WinterSolstice/Camera/OrthographicCamera.h>
#include "Shader.h"
#include <WinterSolstice/Camera/EditorCamera.h>
#include <WinterSolstice/Scene/Components.h>

namespace WinterSolstice {
	namespace Bronya {
		class Renderer
		{
		public:
			struct RenderSubQueue
			{
				std::function<void()> task;
				glm::vec3 zBuffer3;
				bool opacity;
				float zBuffer;
				RenderSubQueue() = default;
			};
			static void Init();
			static void Shutdown();
			static void ResetStats();
			static void OnWindowResize(uint32_t width, uint32_t height);
			static void BeginScene(const Rossweisse::SceneCamera& camera, glm::mat4& transform);
			static void BeginScene(const Rossweisse::OrthographicCamera& camera);
			static void BeginScene(const Rossweisse::EditorCamera& camera);
			static void EndSceneShadowMap();
			static void ExecuteShadowMap();
			static void EndScene();
			static void Flush();


			static void Submit(const std::shared_ptr<VertexArray>& vertexArry, const std::shared_ptr<Shader>& shader);
			static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, int entityID);
			static void SubmitQue(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const std::function<bool()>& fn, const glm::vec3& zBuffer3, bool opacity);
			static void SubmitQueShadowMap(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const std::function<bool()>& fn, const glm::vec3& zBuffer3, bool opacity);
			static void SubmitQueArray(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const std::function<bool()>& fn, const glm::vec3& zBuffer3, bool opacity);
			static void SubmitLine(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader);
			static void SubmitLine(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, int entityID);
			static void SubmitLineQue(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, RenderSubQueue que);

			// Primitives
			static void DrawQuad2D(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
			static void DrawQuad2D(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
			static void DrawQuad2D(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
			static void DrawQuad2D(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

			static void DrawQuad2D(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
			static void DrawQuad2D(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);

			static void DrawRotatedQuad2D(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
			static void DrawRotatedQuad2D(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
			static void DrawRotatedQuad2D(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
			static void DrawRotatedQuad2D(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

			static void DrawCircle2D(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

			static void DrawLine2D(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID = -1);

			static void DrawRect2D(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
			static void DrawRect2D(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

			static void DrawSprite2D(const glm::mat4& transform, Raiden::SpriteRendererComponent& src, int entityID);

			static float GetLineWidth2D();
			static void SetLineWidth2D(float width);

			inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); };
		private:
			struct SceneData
			{
				glm::mat4 ViewProjectionMatrix;
				glm::mat4 ViewMatrix;
				glm::mat4 ProjectionMatrix;
				glm::vec3 CameraPos;
			};
			static SceneData* m_SceneData;
			static std::vector<RenderSubQueue> RenderQueue_opacity;
			static std::vector<RenderSubQueue> RenderQueue_Translucent;

			static std::vector<RenderSubQueue> RenderQueue_opacity_ShadowMap;
			static std::vector<RenderSubQueue> RenderQueue_Translucent_ShadowMap;
		};
	}
}