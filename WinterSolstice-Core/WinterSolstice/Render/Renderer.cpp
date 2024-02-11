#include "k423.h"
#include "Renderer.h"
#include "Renderer2D.h"

#include <thread>
#include <atomic>
#include <semaphore>
#include <condition_variable>
#include <WinterSolstice/Core/True.h>
#include <WinterSolstice/Core/Application.h>
namespace WinterSolstice {
	namespace Bronya {
		Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;
		std::vector<Renderer::RenderSubQueue> Renderer::RenderQueue_opacity;
		std::vector<Renderer::RenderSubQueue> Renderer::RenderQueue_Translucent;
		std::vector<Renderer::RenderSubQueue> Renderer::RenderQueue_opacity_ShadowMap;
		std::vector<Renderer::RenderSubQueue> Renderer::RenderQueue_Translucent_ShadowMap;
		//std::mutex Renderer::QueMutex;
		static True Elysia;

		void Renderer::Init()
		{
			RenderCommand::Init();
			Renderer2D::Init();
		}
		void Renderer::Shutdown()
		{
			Renderer2D::Shutdown();
		}
		void Renderer::OnWindowResize(uint32_t width, uint32_t height)
		{
			RenderCommand::SetViewport(0, 0, width, height);
		}
		void Renderer::BeginScene(const Rossweisse::SceneCamera& camera, glm::mat4& transform)
		{
			m_SceneData->ViewProjectionMatrix = camera.GetProjection() * glm::inverse(transform);
		}
		void Renderer::BeginScene(const Rossweisse::OrthographicCamera& camera)
		{
			m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
		}
		void Renderer::BeginScene(const Rossweisse::EditorCamera& camera)
		{
			RenderQueue_opacity.clear();
			RenderQueue_Translucent.clear();
			m_SceneData->ViewProjectionMatrix = camera.GetViewProjection();
			m_SceneData->ViewMatrix = camera.GetViewMatrix();
			m_SceneData->ProjectionMatrix = camera.GetProjection();
			m_SceneData->CameraPos = camera.GetPosition();
		}
		void Renderer::EndScene()
		{
			//auto result1 = Application::GetThreadPool().AnsycTask([]()
				//{
			std::stable_sort(Bronya::Renderer::RenderQueue_opacity.begin(), Bronya::Renderer::RenderQueue_opacity.end(), [](auto f1, auto f2) {
				return f1.zBuffer > f2.zBuffer;
				});
			//});
		//auto result2 = Application::GetThreadPool().AnsycTask([]()
			//{
			std::stable_sort(Bronya::Renderer::RenderQueue_Translucent.begin(), Bronya::Renderer::RenderQueue_Translucent.end(), [](auto f1, auto f2) {
				return f1.zBuffer > f2.zBuffer;
				});
			//});
		//result1.get();
		//result2.get();
		}
		void Renderer::EndSceneShadowMap()
		{
			//auto result1 = Application::GetThreadPool().AnsycTask([]()
				//{
			std::stable_sort(Bronya::Renderer::RenderQueue_opacity_ShadowMap.begin(), Bronya::Renderer::RenderQueue_opacity_ShadowMap.end(), [](auto f1, auto f2) {
				return f1.zBuffer > f2.zBuffer;
				});
			//});
		//auto result2 = Application::GetThreadPool().AnsycTask([]()
			//{
			std::stable_sort(Bronya::Renderer::RenderQueue_Translucent_ShadowMap.begin(), Bronya::Renderer::RenderQueue_Translucent_ShadowMap.end(), [](auto f1, auto f2) {
				return f1.zBuffer > f2.zBuffer;
				});
			//});
		//result1.get();
		//result2.get();
		}
		void Renderer::Execute()
		{
			for (auto& que : RenderQueue_opacity)
			{
				que.task();
			}
			Renderer2D::Flush();
			for (auto& que : RenderQueue_Translucent)
			{
				que.task();
			}
		}
		void Renderer::ExecuteShadowMap()
		{
			for (auto& que : RenderQueue_opacity_ShadowMap)
			{
				que.task();
			}
			Renderer2D::Flush();
			for (auto& que : RenderQueue_Translucent_ShadowMap)
			{
				que.task();
			}
		}
		void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
		{
			shader->Bind();
			shader->SetMat4("uViewProjection", m_SceneData->ViewProjectionMatrix);
			vertexArray->Bind();
			RenderCommand::DrawIndex(vertexArray);
		}
		void Renderer::SubmitQue(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const std::function<bool()>& fn, const glm::vec3& zBuffer3, bool opacity)
		{
			RenderSubQueue que;
			que.task = [=]() {
				shader->Bind();
				fn();
				shader->SetMat4("uViewProjection", m_SceneData->ViewProjectionMatrix);
				shader->SetMat4("uView", m_SceneData->ViewMatrix);
				shader->SetMat4("uProjection", m_SceneData->ProjectionMatrix);
				vertexArray->Bind();
				RenderCommand::DrawIndex(vertexArray);
				};
			que.zBuffer = Distance(zBuffer3, m_SceneData->CameraPos);
			{
				Elysia.StaffLock();
				!opacity ? RenderQueue_opacity.emplace_back(que) : RenderQueue_Translucent.emplace_back(que);
				Elysia.StaffunLock();
			}
		}
		void Renderer::SubmitQueShadowMap(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const std::function<bool()>& fn, const glm::vec3& zBuffer3, bool opacity)
		{
			RenderSubQueue que;
			que.task = [=]() {
				shader->Bind();
				fn();
				shader->SetMat4("uViewProjection", m_SceneData->ViewProjectionMatrix);
				shader->SetMat4("uView", m_SceneData->ViewMatrix);
				shader->SetMat4("uProjection", m_SceneData->ProjectionMatrix);
				vertexArray->Bind();
				RenderCommand::DrawIndex(vertexArray);
				};
			que.zBuffer = Distance(zBuffer3, m_SceneData->CameraPos);
			{
				Elysia.StaffLock();
				!opacity ? RenderQueue_opacity_ShadowMap.emplace_back(que) : RenderQueue_Translucent_ShadowMap.emplace_back(que);
				Elysia.StaffunLock();
			}
		}
		void Renderer::SubmitQueArray(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const std::function<bool()>& fn, const glm::vec3& zBuffer3, bool opacity)
		{
			RenderSubQueue que;
			que.task = [=]() {
				shader->Bind();
				fn();
				shader->SetMat4("uViewProjection", m_SceneData->ViewProjectionMatrix);
				shader->SetMat4("uView", m_SceneData->ViewMatrix);
				shader->SetMat4("uProjection", m_SceneData->ProjectionMatrix);
				vertexArray->Bind();
				RenderCommand::DrawArrays(vertexArray, 32);
				};
			que.zBuffer = Distance(zBuffer3, m_SceneData->CameraPos);
			{
				Elysia.StaffLock();
				!opacity ? RenderQueue_opacity.emplace_back(que) : RenderQueue_Translucent.emplace_back(que);
				Elysia.StaffunLock();
			}
		}
		void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, int entityID)
		{
			shader->Bind();
			shader->SetMat4("uViewProjection", m_SceneData->ViewProjectionMatrix);
			shader->SetInt("uEntityID", entityID);
			vertexArray->Bind();
			RenderCommand::DrawIndex(vertexArray);
		}
		void Renderer::SubmitLine(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
		{
			shader->Bind();
			shader->SetMat4("uViewProjection", m_SceneData->ViewProjectionMatrix);
			vertexArray->Bind();
			RenderCommand::DrawIndexLines(vertexArray);
		}
		void Renderer::SubmitLine(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, int entityID)
		{
			shader->Bind();
			shader->SetMat4("uViewProjection", m_SceneData->ViewProjectionMatrix);
			shader->SetInt("uEntityID", entityID);
			vertexArray->Bind();
			RenderCommand::DrawIndexLines(vertexArray);
		}
		void Renderer::SubmitLineQue(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, RenderSubQueue que) {
			auto func = que.task;
			que.task = [=]() {
				shader->Bind();
				shader->SetMat4("uViewProjection", m_SceneData->ViewProjectionMatrix);
				func();
				vertexArray->Bind();
				RenderCommand::DrawIndexLines(vertexArray);
				};
			que.zBuffer = Distance(que.zBuffer3, m_SceneData->CameraPos);
			{
				Elysia.StaffLock();
				RenderQueue_opacity.push_back(que);
				Elysia.StaffunLock();
			}
		}
		void Renderer::DrawQuad2D(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
		{
			Renderer2D::DrawQuad(position, size, color);
		}
		void Renderer::DrawQuad2D(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
		{
			Renderer2D::DrawQuad(position, size, color);
		}
		void Renderer::DrawQuad2D(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
		{
			Renderer2D::DrawQuad(position, size, texture, tilingFactor, tintColor);
		}
		void Renderer::DrawQuad2D(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
		{
			Renderer2D::DrawQuad(position, size, texture, tilingFactor, tintColor);
		}
		void Renderer::DrawQuad2D(const glm::mat4& transform, const glm::vec4& color, int entityID)
		{
			Renderer2D::DrawQuad(transform, color, entityID);
		}
		void Renderer::DrawQuad2D(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
		{
			Renderer2D::DrawQuad(transform, texture, tilingFactor, tintColor, entityID);
		}
		void Renderer::DrawRotatedQuad2D(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
		{
			Renderer2D::DrawRotatedQuad(position, size, rotation, color);
		}
		void Renderer::DrawRotatedQuad2D(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
		{
			Renderer2D::DrawRotatedQuad(position, size, rotation, color);
		}
		void Renderer::DrawRotatedQuad2D(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
		{
			Renderer2D::DrawRotatedQuad(position, size, rotation, texture, tilingFactor, tintColor);
		}
		void Renderer::DrawRotatedQuad2D(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
		{
			Renderer2D::DrawRotatedQuad(position, size, rotation, texture, tilingFactor, tintColor);
		}
		void Renderer::DrawCircle2D(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
		{
			Renderer2D::DrawCircle(transform, color, thickness, fade, entityID);
		}
		void Renderer::DrawLine2D(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
		{
			Renderer2D::DrawLine(p0, p1, color, entityID);
		}
		void Renderer::DrawRect2D(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
		{
			Renderer2D::DrawRect(position, size, color, entityID);
		}
		void Renderer::DrawRect2D(const glm::mat4& transform, const glm::vec4& color, int entityID)
		{
			Renderer2D::DrawRect(transform, color, entityID);
		}
		void Renderer::DrawSprite2D(const glm::mat4& transform, Raiden::SpriteRendererComponent& src, int entityID)
		{
			Renderer2D::DrawSprite(transform, src, entityID);
		}
		float Renderer::GetLineWidth2D()
		{
			return Renderer2D::GetLineWidth();
		}
		void Renderer::SetLineWidth2D(float width)
		{
			Renderer2D::SetLineWidth(width);
		}
	}
}