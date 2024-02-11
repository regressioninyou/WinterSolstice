#include "k423.h"
#include "Scene.h"
#include "Entity.h"
#include <glm/glm.hpp>
#include "WinterSolstice/Render/Renderer2D.h"
#include "Components.h"
#include "WinterSolstice/Object/Loder.h"
#include "WinterSolstice/Render/Buffer.h"
#include <WinterSolstice/Render/Renderer.h>
#include "WinterSolstice/Utils/PlatformUtils.h"
#include "WinterSolstice/Object/DrawCall.h"
#include <WinterSolstice/Utils/ThreadPool.h>
#include <WinterSolstice/Core/Application.h>
#include "WinterSolstice/Core/True.h"
namespace WinterSolstice {
	namespace Raiden {

		static True Elysia;


		static void DoMath(const glm::mat4& transform)
		{

		}

		static void OnTransformConstruct(entt::registry& registry, entt::entity entity)
		{

		}

		Scene::Scene()
		{
			root = CreateRef<ListNode>();
			Entity entity(m_Registry.create(), this);
			entity.AddComponent<TransformComponent>();
			auto& tag = entity.AddComponent<TagComponent>();
			tag.Tag = "Scene";
			root->entity = entity;

		}

		Scene::~Scene()
		{
		}

		Entity Scene::CreateEntity(const std::string& name)
		{
			Ref<ListNode> node = CreateRef<ListNode>();
			Entity entity(m_Registry.create(), this);
			node->entity = entity.Get();
			root->child.push_back(node);

			entity.AddComponent<TransformComponent>();
			auto& tag = entity.AddComponent<TagComponent>();
			tag.Tag = name.empty() ? "Entity" : name;
			return entity;
		}

		void Scene::DestoryAllChild(Entity entity, std::pair<int, Ref<Scene::ListNode>> pair) {
			if (entity) {
				if (entity.HasComponent<NodeComponent>()) {
					auto& node = entity.GetComponent<NodeComponent>();
					auto child = node.current;
					if (child) {
						//uint32_t size = child->child.size();
						for (uint32_t i = 0; i < child->child.size(); i++) {
							DestoryAllChild(Entity(child->child[i]->entity, this), { i,child });
						}
					}
				}
				pair.second->child.erase(pair.second->child.begin() + pair.first);
				m_Registry.destroy(entity);
			}
		}
		void Scene::DestoryEntity(Entity entity)
		{
			auto pair = ListAt(entity);
			DestoryAllChild(entity, pair);
		}

		void Scene::OnViewportResize(uint32_t width, uint32_t height)
		{
			m_ViewportWidth = width;
			m_ViewportHeight = height;

			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto& camera = view.get<CameraComponent>(entity);
				if (!camera.FixedAspectRatio)
				{
					camera.camera.SetViewportSize(width, height);
				}
			}
		}
		void Scene::OnUpdateEditor(Kiana::Timestep ts, Rossweisse::EditorCamera& camera)
		{
			RenderScene(camera);
		}
		void Scene::OnUpdateRuntime(Kiana::Timestep ts)
		{
			{
				m_Registry.view<NativeScriptComponent>().each([&](auto entity, auto& nsc)
					{
						if (!nsc.Instance)
						{
							nsc.Instance = nsc.InstantiateScript();
							nsc.Instance->m_Entity = Entity{ entity,this };
							nsc.Instance->OnCreate();
						}
						nsc.Instance->OnUpdate(ts);
					});
			}
			Rossweisse::SceneCamera* mainCamera = nullptr;
			glm::mat4 cameraTransform;
			{
				auto view = m_Registry.view<TransformComponent, CameraComponent>();
				for (auto entity : view)
				{
					auto& transform = view.get<TransformComponent>(entity);
					auto& camera = view.get<CameraComponent>(entity);
					if (camera.Primary)
					{
						mainCamera = &camera.camera;
						cameraTransform = transform.GetTransform();
						break;
					}
				}
			}

			if (mainCamera)
			{
				// Draw sprites
				Bronya::Renderer2D::BeginScene(*mainCamera, cameraTransform);
				{
					auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
					for (auto entity : group)
					{
						auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

						Bronya::Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
					}
				}

				// Draw circles
				{
					auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
					for (auto entity : view)
					{
						auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

						Bronya::Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
					}
				}
				Bronya::Renderer2D::EndScene();
				Bronya::Renderer::BeginScene(*mainCamera, cameraTransform);
				{
					{
						auto tag = m_Registry.view<TagComponent>();
						for (auto ta : tag)
						{
							auto t = tag.get<TagComponent>(ta);
							//KyBao_CORE_INFO("Name : {0}",t.Tag);
						}
					}
					auto Obj = m_Registry.view<TransformComponent, ObjectComponent, MaterialComponent>();
					for (auto child : Obj)
					{
						auto [transform, m, s] = Obj.get<TransformComponent, ObjectComponent, MaterialComponent>(child);
						auto mesh = m.object;
						auto shader = s.material;
						if (mesh->GetObjectClass() == ObjectClass::Mesh)
						{
							DrawCall* drawCall = dynamic_cast<DrawCall*>(mesh.get());

							auto task = [=]()->bool {
								shader->Get("Blue")->SetMat4("uModel", transform.GetTransform());
								drawCall->BindTexture(shader->Get("Blue"));
								return drawCall->isReady();
								};
							Bronya::Renderer::SubmitQue(drawCall->getVertexArray(), shader->Get("Blue"), task, mesh->GetZBuffer(), drawCall->isTranslucent());

							//shader->Get("Blue")->Bind();
							//shader->Get("Blue")->SetMat4("uModel", transform.GetTransform());
							//Bronya::Renderer::Submit(drawCall->getVertexArray(), shader->Get("Blue"));

							//shader->Get("Tirangle")->Bind();
							//shader->Get("Tirangle")->SetMat4("uModel", transform.GetTransform());
							//Bronya::Renderer::SubmitLine(drawCall->getVertexArray(), shader->Get("Tirangle"));
							//mesh->BVHDrawBounds();
						}
					}
				}
				Bronya::Renderer::EndScene();

			}
		}

		Entity Scene::GetPrimaryCamera()
		{
			auto view = m_Registry.view<CameraComponent>();
			for (auto entity : view)
			{
				auto camera = view.get<CameraComponent>(entity);
				if (camera.Primary)
				{
					return Entity(entity, this);
				}
			}
			return Entity();
		}

		Entity Scene::AppendChild(Ref<ListNode> node, const std::string& name)
		{
			Ref<ListNode> child = CreateRef<ListNode>();
			Entity entity(m_Registry.create(), this);
			child->entity = entity.Get();

			node->child.push_back(child);

			entity.AddComponent<TransformComponent>();
			auto& tag = entity.AddComponent<TagComponent>();
			tag.Tag = name.empty() ? "Entity" : name;
			return entity;
		}
		static std::pair<int, Ref<Raiden::Scene::ListNode>> EachThree(Ref<Scene::ListNode> parent, entt::entity node) {
			if (parent->child.empty() || parent == nullptr) return { -1, nullptr };
			std::pair<int, Ref<Scene::ListNode>> result = { -1,nullptr };
			for (uint32_t i = 0; i < parent->child.size(); i++)
			{
				if (parent->child[i]->entity == node)
					return { i ,parent };
				auto r = EachThree(parent->child[i], node);
				if (r.first != -1)
					result = r;
			}
			return result;
		}

		std::pair<int, Ref<Raiden::Scene::ListNode>> Scene::ListAt(entt::entity node)
		{
			Elysia.PlumeLock();
			static std::unordered_map<entt::entity, std::pair<int, Ref<Raiden::Scene::ListNode>>> ListMap;
			if (ListMap.find(node) == ListMap.end()) {
				auto result = EachThree(root, node);
				ListMap[node] = result;
			}
			Elysia.PlumeUnLock();
			return ListMap[node];
		}
		void Scene::AddTaskResult(std::function<int(Ref<Scene::ListNode>child, Scene* scene)> func, Ref<Scene::ListNode>& child)
		{
			Elysia.PlumeLock();
			taskResults.push_back(Application::GetThreadPool().AnsycTaskShared(func, child, this));
			Elysia.PlumeUnLock();
		}
		void Scene::SubmitEntity3D(Ref<Scene::ListNode>& root, TransformComponent translation, int depth) {
			auto func = [this, root, translation, depth](Ref<Scene::ListNode> child, Scene* scene)->int {
				Entity entity(child->entity, scene);
				if (entity.HasComponent<ObjectComponent>()) {
					//auto [transform, m] = m_Registry.get<TransformComponent, ObjectComponent>(child->entity);
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& m = entity.GetComponent<ObjectComponent>();
					auto& mesh = m.object;
					auto t = transform + translation;
					//auto& obj = entity.GetComponent<ObjectComponent>().object;
					//mesh->SetTranslation({ t.Translation,t.Rotation,t.Scale });
					//scene->SubmitEntity3D(child, t, depth + 1);
					if (mesh->GetObjectClass() == ObjectClass::Mesh)
					{
						auto& s = entity.GetComponent<MaterialComponent>();
						auto& shaders = s.material;
						auto trans = t.GetTransform();
						DrawCall* drawCall = dynamic_cast<DrawCall*>(mesh.get());
						{
							//if (shaders->) {
							for (auto& shader : *shaders) {
								auto material = shader.second;
								auto task = [material, drawCall, trans]()->bool {
									material->SetMat4("uModel", trans);
									drawCall->BindTexture(material);
									return drawCall->isReady();
									};
								Bronya::Renderer::SubmitQue(drawCall->getVertexArray(), material, task, mesh->GetZBuffer(), drawCall->isTranslucent());
								//}
							}
						}
						//{
						//	Renderer::RenderSubQueue renderque;
						//	renderque.task = [=]() {
						//		shader->Get("Tirangle")->SetMat4("uModel", trans);
						//		drawCall->BindTexture(shader->Get("Tirangle"));
						//		};
						//	renderque.zBuffer3 = mesh->GetZBuffer();
						//	KyBao::Renderer::SubmitLineQue(drawCall->getVertexArray(), shader->Get("Tirangle"), renderque);
						//}
						//shader->Get("Tirangle")->Bind();
						//shader->Get("Tirangle")->SetMat4("uModel", t.GetTransform());
						//KyBao::Renderer::SubmitLine(drawCall->getVertexArray(), shader->Get("Tirangle"));
					}
					else {
						scene->SubmitEntity3D(child, t, depth + 1);
					}
					//mesh->BVHDrawBounds();
					//维护obj
				}
				else {
					auto transform = scene->m_Registry.get<TransformComponent>(entity);
					auto t = transform + translation;
					if (entity.HasComponent<CircleRendererComponent>())
					{
						auto circle = scene->m_Registry.get<CircleRendererComponent>(entity);
						Bronya::Renderer2D::DrawCircle(t.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
					}
					else if (entity.HasComponent<SpriteRendererComponent>()) {
						auto sprite = scene->m_Registry.get<SpriteRendererComponent>(entity);
						Bronya::Renderer2D::DrawSprite(t.GetTransform(), sprite, (int)entity);
					}
					scene->SubmitEntity3D(child, t, depth + 1);
				}
				return 1;
				};


#if 0
			for (auto child : root->child)
				AddTaskResult(func, child);
#else // MoreAndMore
			if (depth >= 1) {
				std::vector<std::future<int>> fints;
				for (auto& child : root->child)
					fints.push_back(Application::GetThreadPool().AnsycTask(func, child, this));
				for (auto& fint : fints) {
					fint.get();
				}
			}
			else
				for (auto child : root->child)
					func(child, this);
#endif
		}
		void Scene::SubmitEntityShadowMap3D(Ref<Scene::ListNode>& root, TransformComponent translation, int depth) {
			auto func = [this, root, translation, depth](Ref<Scene::ListNode> child, Scene* scene)->int {
				Entity entity(child->entity, scene);
				if (entity.HasComponent<ObjectComponent>()) {
					//auto [transform, m] = m_Registry.get<TransformComponent, ObjectComponent>(child->entity);
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& m = entity.GetComponent<ObjectComponent>();
					auto& mesh = m.object;
					auto t = transform + translation;
					//auto& obj = entity.GetComponent<ObjectComponent>().object;
					//mesh->SetTranslation({ t.Translation,t.Rotation,t.Scale });
					//scene->SubmitEntity3D(child, t, depth + 1);
					if (mesh->GetObjectClass() == ObjectClass::Mesh)
					{
						auto& s = entity.GetComponent<MaterialComponent>();
						auto& shaders = s.material;
						auto trans = t.GetTransform();
						DrawCall* drawCall = dynamic_cast<DrawCall*>(mesh.get());
						{
							//if (shaders->) {
							for (auto& shader : *shaders) {
								auto material = shader.second;
								auto task = [material, drawCall, trans]()->bool {
									material->SetMat4("uModel", trans);
									drawCall->BindTexture(material);
									return drawCall->isReady();
									};
								Bronya::Renderer::SubmitQueShadowMap(drawCall->getVertexArray(), material, task, mesh->GetZBuffer(), drawCall->isTranslucent());
								//}
							}
						}
						//{
						//	Renderer::RenderSubQueue renderque;
						//	renderque.task = [=]() {
						//		shader->Get("Tirangle")->SetMat4("uModel", trans);
						//		drawCall->BindTexture(shader->Get("Tirangle"));
						//		};
						//	renderque.zBuffer3 = mesh->GetZBuffer();
						//	KyBao::Renderer::SubmitLineQue(drawCall->getVertexArray(), shader->Get("Tirangle"), renderque);
						//}
						//shader->Get("Tirangle")->Bind();
						//shader->Get("Tirangle")->SetMat4("uModel", t.GetTransform());
						//KyBao::Renderer::SubmitLine(drawCall->getVertexArray(), shader->Get("Tirangle"));
					}
					else {
						scene->SubmitEntity3D(child, t, depth + 1);
					}
					//mesh->BVHDrawBounds();
					//维护obj
				}
				else {
					auto transform = scene->m_Registry.get<TransformComponent>(entity);
					auto t = transform + translation;
					if (entity.HasComponent<CircleRendererComponent>())
					{
						auto circle = scene->m_Registry.get<CircleRendererComponent>(entity);
						Bronya::Renderer2D::DrawCircle(t.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
					}
					else if (entity.HasComponent<SpriteRendererComponent>()) {
						auto sprite = scene->m_Registry.get<SpriteRendererComponent>(entity);
						Bronya::Renderer2D::DrawSprite(t.GetTransform(), sprite, (int)entity);
					}
					scene->SubmitEntity3D(child, t, depth + 1);
				}
				return 1;
				};


#if 0
			for (auto child : root->child)
				AddTaskResult(func, child);
#else // MoreAndMore
			if (depth >= 1) {
				std::vector<std::future<int>> fints;
				for (auto& child : root->child)
					fints.push_back(Application::GetThreadPool().AnsycTask(func, child, this));
				for (auto& fint : fints) {
					fint.get();
				}
			}
			else
				for (auto child : root->child)
					func(child, this);
#endif
		}
		void Scene::SetDefaultImage(const std::string& defualtPath)
		{
			DefaultTexture = Bronya::RenderCommand::GetTexture2D(defualtPath);
		}
		void Scene::SetDefaultImage(Ref<Bronya::Texture2D> defualtPath)
		{
			DefaultTexture = defualtPath;
		}
		Ref<Bronya::Texture2D> Scene::GetDefaultImage()
		{
			return DefaultTexture;
		}
		void Scene::RenderScene(Rossweisse::EditorCamera& camera)
		{
			Bronya::Renderer2D::BeginScene(camera);
			Bronya::Renderer::BeginScene(camera);
			//auto translation = m_Registry.get<TransformComponent>(root->entity);

			RecaculateTransformNode();
			SubmitRenderMesh();
			//SubmitEntity3D(root, translation);
			Bronya::Renderer::EndScene();
			Bronya::Renderer2D::EndScene();
		}

		void Scene::RecaculateTransformNode()
		{
			WaitRecaculateTransformNode(root, glm::mat4(1.0f));
			for (auto& get : WaitTransformTask) {
				if (get.valid()) {
					get.get();
				}
			}
			WaitTransformTask.clear();
		}

		void Scene::WaitRecaculateTransformNode(Ref<Scene::ListNode>& root, glm::mat4 translation, int depth)
		{
			auto func = [this, translation, depth](Ref<Scene::ListNode> child, Scene* scene)->int {
				Entity entity(child->entity, scene);
				{
					std::lock_guard<std::recursive_mutex> lock(AddTransformMapMutex);
					m_TransformMat[(uint32_t)child->entity] = translation;
				}
				TransformComponent trans = entity.GetComponent<TransformComponent>();
				auto t = trans.GetTransform() * translation;

				WaitRecaculateTransformNode(child, t, depth + 1);
				return 1;
				};
			for (auto child : root->child)
			{
				std::lock_guard<std::recursive_mutex> lock(WaitTransformTaskMutex);
				WaitTransformTask.emplace_back(Application::GetThreadPool().AnsycTask(func, child, this));
			}
		}

		void Scene::SubmitRenderMesh()
		{
			{
				auto group = m_Registry.group<ObjectComponent>(entt::get<TransformComponent>);
				for (auto& entity : group) {
					auto [transform, object] = group.get<TransformComponent, ObjectComponent>(entity);
					if (object.object->GetObjectClass() == ObjectClass::Mesh) {
						auto mesh = object.object;
						WaitRenderTasks.emplace_back(Application::GetThreadPool().AnsycTask([entity, this, mesh]()->int
							{
								auto it = m_TransformMat.find((uint32_t)entity);
								if (it != m_TransformMat.end()) {
									Entity comp(entity, this);
									auto& s = comp.GetComponent<MaterialComponent>();
									auto& shaders = s.material;
									auto trans = comp.GetComponent<TransformComponent>().GetTransform() * it->second;
									DrawCall* drawCall = dynamic_cast<DrawCall*>(mesh.get());
									{
										//if (shaders->) {
										for (auto& shader : *shaders) {
											auto material = shader.second;
											auto task = [material, drawCall, trans]()->bool {
												material->SetMat4("uModel", trans);
												drawCall->BindTexture(material);
												return drawCall->isReady();
												};
											Bronya::Renderer::SubmitQue(drawCall->getVertexArray(), material,
												task, mesh->GetZBuffer(), drawCall->isTranslucent());
										}
									}
								}
								return 1;
							}));
					}
				}
			}

			//{
			//	auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			//	for (auto entity : group)
			//	{
			//		auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			//		auto it = m_TransformMat.find((uint32_t)entity);
			//		if (it != m_TransformMat.end()) {
			//			auto trans = transform.GetTransform() * it->second;
			//			Bronya::Renderer2D::DrawSprite(trans, sprite, (int)entity);
			//		}
			//	}
			//}
			// Draw circles
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
					auto it = m_TransformMat.find((uint32_t)entity);
					if (it != m_TransformMat.end()) {
						auto trans = transform.GetTransform() * it->second;
						Bronya::Renderer2D::DrawCircle(trans, circle.Color, circle.Thickness, circle.Fade, (int)entity);
					}
				}
			}
			for (auto& task : WaitRenderTasks) {
				if (task.valid()) {
					task.get();
				}
			}
			WaitRenderTasks.clear();

		}

		template<typename T>
		void Scene::OnComponentAdd(Entity entity, T& component)
		{
			//static_assert(false);
		}

		template<>
		void Scene::OnComponentAdd<TagComponent>(Entity entity, TagComponent& component)
		{

		}

		template<>
		void Scene::OnComponentAdd<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
		{

		}

		template<>
		void Scene::OnComponentAdd<TransformComponent>(Entity entity, TransformComponent& component)
		{

		}

		template<>
		void Scene::OnComponentAdd<CameraComponent>(Entity entity, CameraComponent& component)
		{
			component.camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		}

		template<>
		void Scene::OnComponentAdd<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
		{

		}

		template<>
		void Scene::OnComponentAdd<NodeComponent>(Entity entity, NodeComponent& component)
		{

		}

		template<>
		void Scene::OnComponentAdd<ObjectComponent>(Entity entity, ObjectComponent& component)
		{
			//std::string str = FileDialogs::OpenFile("KyBao Scene");
			//if (!str.empty())
			{
				//component.object = Loder::Get()->loadOBJ(str);
			}
		}

		template<>
		void Scene::OnComponentAdd<MaterialComponent>(Entity entity, MaterialComponent& component)
		{
			//std::string str = FileDialogs::OpenFile("KyBao Scene (*.glsl)\0*.glsl\0");
			//if (!str.empty())
			{
				//component.material = Shader::Create(str);
			}
		}
	}
}