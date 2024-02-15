#pragma once
#include <WinterSolstice/Core/Timestep.h>
#include "Platform/load/entt/entt.hpp"
#include "WinterSolstice/Camera/EditorCamera.h"
#include <string>
#include <WinterSolstice/Object/Object.h>
#include <WinterSolstice/Animation/Bone.h>
#include <future>
#include <queue>
namespace WinterSolstice {
	namespace Bronya {
		class Texture2D;
	}
	namespace Raiden {
		class Entity;
		class SynchronizeMultiThreadedPool;
		struct NodeComponent;
		struct TransformComponent;
		class Scene
		{
		public:
			Scene();
			~Scene();

			Entity CreateEntity(const std::string& name = std::string());
			void DestoryEntity(Entity entity);

			void OnViewportResize(uint32_t width, uint32_t height);

			void OnUpdateEditor(Kiana::Timestep ts, Rossweisse::EditorCamera& camera);
			void OnUpdateRuntime(Kiana::Timestep ts);

			Entity GetPrimaryCamera();
			struct ListNode
			{
				entt::entity entity;
				std::vector<Ref<ListNode>> child;
			};
			Entity AppendChild(Ref<ListNode> node, const std::string& name = std::string());
			Ref<ListNode>& GetRoot() { return root; }

			std::pair<int, Ref<Scene::ListNode>> ListAt(entt::entity node);

			Ref<Bone> rootBone = nullptr;
			void SetDefaultImage(const std::string& defualtPath);
			void SetDefaultImage(Ref<Bronya::Texture2D> defualtPath);

			Ref<Bronya::Texture2D> GetDefaultImage();
		private:
			template<typename T>
			void OnComponentAdd(Entity entity, T& component);

			void RenderScene(Rossweisse::EditorCamera& camera);
			void RecaculateTransformNode();
			void WaitRecaculateTransformNode(Ref<Scene::ListNode>& root, glm::mat4 translation, int depth = 0);
			void SubmitRenderMesh();

			void DestoryAllChild(Entity entity, std::pair<int, Ref<Scene::ListNode>> pair);

			void AddTaskResult(std::function<int(Ref<Scene::ListNode> child, Scene* scene)> func, Ref<Scene::ListNode>& child);
		private:
			Ref<ListNode> root;
			entt::registry m_Registry;
			uint32_t m_ViewportWidth, m_ViewportHeight;
			std::deque<Ref<std::packaged_task<int()>>> taskResults;
			Ref<Bronya::Texture2D> DefaultTexture;

			std::unordered_map<uint32_t, glm::mat4> m_TransformMat;
			std::vector<std::shared_future<int>> WaitTransformTask;
			std::recursive_mutex WaitTransformTaskMutex;
			std::recursive_mutex AddTransformMapMutex;

			std::vector<std::shared_future<int>> WaitRenderTasks;
			std::mutex WaitRenderMutex;

			friend class Entity;
			friend class SceneHierarchyPanel;
			friend class SceneSerializer;
		};
	}
}