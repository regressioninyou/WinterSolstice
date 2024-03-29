#pragma once 
#include <WinterSolstice/Core/Core.h>
#include <WinterSolstice/Scene/Scene.h>
#include <WinterSolstice/Scene/Entity.h>
namespace WinterSolstice {
	namespace Bronya {
		class Shader;
	}
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel();
		SceneHierarchyPanel(const Ref<Raiden::Scene>& scene);
		
		void SetContext(const Ref<Raiden::Scene>& scene);
		void OnImGuiRender();

		Raiden::Entity SeletedEntity() { return m_SelectionContext; };
		void SetSelectedEntity(Raiden::Entity entity);
	private:
		void DrawEntityNode(Raiden::Entity entity,std::vector<Ref<Raiden::Scene::ListNode>>::iterator& iterat);
		void DrawComponents(Raiden::Entity entity);
	private:
		Ref<Bronya::Shader> Computed;
		Ref<Raiden::Scene> m_Context;
		Raiden::Entity m_SelectionContext;
		std::unordered_map<std::string, std::pair<std::function<void()>, bool>> Render_Que;
		template<typename T>
		friend void HasAddComponent(const char* name, SceneHierarchyPanel& sh);
	};
}