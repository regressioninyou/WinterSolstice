#pragma once
#include "Scene.h"
#include "Platform/load/entt/entt.hpp"
namespace WinterSolstice {
	namespace Raiden {
		class Entity
		{
		public:
			enum class EntityType {
				Root = 0, Node = 1
			};
			Entity() = default;
			Entity(entt::entity handle, Scene* scene);
			Entity(uint32_t handle, Scene* scene);


			template<typename T, typename ...Args>
			T& AddComponent(Args && ...args)
			{
				if (HasComponent<T>())
					return GetComponent<T>();
				T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
				m_Scene->OnComponentAdd<T>(*this, component);
				return component;
			}

			template<typename T>
			T& GetComponent()
			{
				Kiana_CORE_ASSERT(HasComponent<T>(), "Enity does not have component");
				return m_Scene->m_Registry.get<T>(m_EntityHandle);
			}
			template<typename T>
			bool HasComponent()
			{
				return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
			}

			template<typename T>
			void RemoveComponent()
			{
				Kiana_CORE_ASSERT(!HasComponent<T>(), "Enity does not have component");
				m_Scene->m_Registry.remove<T>(m_EntityHandle);
			}
			bool isNull()const
			{
				return m_EntityHandle == entt::null;
			}

			entt::entity Get() {
				Kiana_CORE_ASSERT(isNull(), "Enity does not have component");
				return m_EntityHandle;
			}
			operator entt::entity() const
			{
				if (isNull())
				{

					Kiana_CORE_ASSERT(isNull(), "Enity does not have component");
				}
				return m_EntityHandle;
			};
			operator bool() const {
				Kiana_CORE_ASSERT(isNull(), "Enity does not have component");
				return m_EntityHandle != entt::null;
			}
			operator uint32_t() const {
				return (uint32_t)m_EntityHandle;
			}
			bool operator==(const Entity& ortho) const {
				return m_EntityHandle == ortho.m_EntityHandle && m_Scene == ortho.m_Scene;
			}
			bool operator!=(const Entity& ortho) const { return !(*this == ortho); }


			Entity AppendChild(const std::string& name = std::string());
		protected:
			EntityType type = EntityType::Root;
			entt::entity m_EntityHandle = entt::null;
			NodeComponent* m_Node;
			Scene* m_Scene;
		};
	}
}
