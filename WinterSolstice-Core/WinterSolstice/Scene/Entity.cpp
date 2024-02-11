#include "k423.h"
#include "Entity.h"
#include "Components.h"
namespace WinterSolstice {
	namespace Raiden {
		Entity::Entity(entt::entity handle, Scene* scene)
			: m_EntityHandle(handle), m_Scene(scene)
		{
		}

		Entity Entity::AppendChild(const std::string& name) {
			auto& child = AddComponent<NodeComponent>();
			auto at = m_Scene->ListAt(m_EntityHandle);

			child.current = at.second->child[at.first];

			return m_Scene->AppendChild(child.current, name);
		}
	}
}