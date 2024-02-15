#include "k423.h"
#include "SceneHierarchyPanel.h"
#include <WinterSolstice.h>
#include <filesystem>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include "WinterSolstice/Utils/PlatformUtils.h"
#include "WinterSolstice/Scene/Components.h"
namespace WinterSolstice {
	SceneHierarchyPanel::SceneHierarchyPanel()
	{
		Computed = Bronya::Shader::CreateCompute("./assets/shaders/compute_shader.glsl");
	}
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Raiden::Scene>& scene)
	{
		Computed = Bronya::Shader::CreateCompute("./assets/shaders/compute_shader.glsl");
		SetContext(scene);
	}
	void SceneHierarchyPanel::SetContext(const Ref<Raiden::Scene>& scene)
	{
		m_Context = scene;
		m_SelectionContext = {};
	}
	template<typename T>
	static void HasAddComponent(const char* name, SceneHierarchyPanel& sh) {
		if (!sh.m_SelectionContext.HasComponent<T>() && ImGui::MenuItem(name))
		{
			sh.m_SelectionContext.AddComponent<T>();
			ImGui::CloseCurrentPopup();
		}
	}
	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		if (m_Context)
		{
			if (ImGui::BeginPopupContextWindow("table Seleted", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
			{
				if (ImGui::MenuItem("Create Empty Entity")) {
					m_Context->CreateEntity("Empty Entity");
				}

				ImGui::EndPopup();
			}

			auto root = m_Context->GetRoot();
			for (auto it = root->child.begin(); it != root->child.end(); ++it)
			{
				Raiden::Entity entity{ it->get()->entity,m_Context.get() };
				DrawEntityNode(entity, it);
			}
			root->child.erase(
				std::remove_if(root->child.begin(), root->child.end(),
					[](const Ref<Raiden::Scene::ListNode>& o) { return !o; }),
				root->child.end()
			);
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				m_SelectionContext = {};
			}
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}
		ImGui::End();
		for (auto it = Render_Que.begin(); it != Render_Que.end(); /* 注意：迭代器在循环内部更新 */) {
			if (it->second.second) {
				it->second.first();
				++it; // 指向下一个元素
			}
			else {
				it = Render_Que.erase(it); // 删除当前元素，并返回下一个元素的迭代器
			}
		}
	}
	void SceneHierarchyPanel::SetSelectedEntity(Raiden::Entity entity)
	{
		m_SelectionContext = entity;
	}
	void SceneHierarchyPanel::DrawEntityNode(Raiden::Entity entity, std::vector<Ref<Raiden::Scene::ListNode>>::iterator& iterat)
	{
		auto& tag = entity.GetComponent<Raiden::TagComponent>().Tag;
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());


		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		bool entityAppend = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Append Entity"))
				entityAppend = true;

			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed;
			//bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			auto pair = m_Context->ListAt(entity.Get());
			auto now = pair.second->child[pair.first];
			for (auto it = now->child.begin(); it != now->child.end(); ++it)
			{
				Raiden::Entity entity{ it->get()->entity,m_Context.get() };
				DrawEntityNode(entity, it);
			}
			now->child.erase(
				std::remove_if(now->child.begin(), now->child.end(),
					[](const Ref<Raiden::Scene::ListNode>& o) { 
						return !o; 
					}),
				now->child.end()
			);
			//if (opened)
				//ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityAppend) {
			m_SelectionContext = entity.AppendChild();
		}
		if (entityDeleted)
		{
			m_Context->DestoryEntity(entity);
			iterat->reset();
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}
	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.01f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.01f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.01f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string name, Raiden::Entity entity, UIFunction function)
	{
		constexpr ImGuiTreeNodeFlags  treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		auto& component = entity.GetComponent<T>();
		ImVec2 contentRegionAvailabel = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(),
			treeNodeFlags, name.c_str());
		ImGui::PopStyleVar();
		ImGui::SameLine(contentRegionAvailabel.x - lineHeight * 0.5f);
		ImGui::SetWindowFontScale(1.1f);
		if (ImGui::Button("+", ImVec2{ lineHeight,lineHeight }))
		{
			ImGui::OpenPopup("ComponentSettings");
		}
		ImGui::SetWindowFontScale(1.0f);
		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove Component"))
				removeComponent = true;
			ImGui::EndPopup();
		}
		if (open)
		{
			function(component);
			ImGui::TreePop();
		}
		if (removeComponent)
		{
			entity.RemoveComponent<T>();
		}
	}

	static void ShowImage(Ref<Bronya::Texture2D> texture, DrawCall* drawcall, Ref<Bronya::Shader> Computed, std::function<void()> callback = []() {}) {
		if (ImGui::Button("replace", ImVec2(100.0f, 0.0f))) {
			std::string str = FileDialogs::OpenFile("KyBao Scene  (*.*)\0*.*\0");
			if (!str.empty())
			{
				Ref<Bronya::Texture2D> tex = Bronya::RenderCommand::GetTexture2D(str);
				//tex->setName("Diffuse");
				//tex->setPath(str);
				tex->setType(texture->getType());

				Computed->GenComputeResultBuffer(sizeof(int));
				Computed->Bind();
				tex->Bind();
				Computed->SetInt("texWidth", tex->GetWidth());
				Computed->SetInt("texHeight", tex->GetHeight());
				Computed->StartCompute();
				int* result = new int;
				Computed->GetComputeResult(result);
				if (*result == 1) {
					tex->SetTranslucent(true);
				}
				delete result;
				texture->SetShaderUsing(CASTUINTPTR(drawcall), false);
				tex->SetShaderUsing(CASTUINTPTR(drawcall), true);
				drawcall->ResetTexture(texture, tex);
				//it->second.second = false;
				callback();
			}
		}
	}
	void SceneHierarchyPanel::DrawComponents(Raiden::Entity entity)
	{
		if (entity.HasComponent<Raiden::TagComponent>())
		{
			auto& tag = entity.GetComponent<Raiden::TagComponent>().Tag;
			char buf[255];
			memset(buf, 0, sizeof(buf));
			strcpy_s(buf, sizeof(buf), tag.c_str());
			if (ImGui::InputText("##Tag", buf, sizeof(buf)))
			{
				tag = std::string(buf);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			HasAddComponent<Raiden::CameraComponent>("Camera", *this);
			HasAddComponent<Raiden::TransformComponent>("Transform", *this);
			HasAddComponent<Raiden::SpriteRendererComponent>("Sprite Render", *this);
			HasAddComponent<Raiden::CircleRendererComponent>("Cricle Render", *this);
			HasAddComponent<Raiden::TextureRendererComponent>("Texture Render", *this);
			HasAddComponent<Raiden::MaterialComponent>("Material", *this);
			HasAddComponent<Raiden::ObjectComponent>("Object", *this);
			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		if (entity.HasComponent<Raiden::TransformComponent>())
			DrawComponent<Raiden::TransformComponent>("Tranform", entity, [](auto& component)
				{
					DrawVec3Control("Translation", component.Translation);

					DrawVec3Control("Rotation", component.Rotation);

					DrawVec3Control("Scale", component.Scale, 1.0f);
				});

		if (entity.HasComponent<Raiden::MaterialComponent>())
			DrawComponent<Raiden::MaterialComponent>("Material", entity, [](auto& component)
				{
					Ref<Bronya::ShaderLibrary> material = component.material;
					for (auto& m : *material) {
						ImGui::Text(m.first.c_str());
					}
				});

		if (entity.HasComponent<Raiden::CameraComponent>())
			DrawComponent<Raiden::CameraComponent>("Camera", entity, [](auto& component)
				{
					auto& camera = component.camera;

					ImGui::Checkbox("Primary", &component.Primary);

					const char* projectionTypeString[] = { "Perspective","OrthoGraphics" };
					const auto* currentProjectionTypeString = projectionTypeString[(int)component.camera.GetProjectType()];
					if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
					{
						for (size_t i = 0; i < 2; i++)
						{
							bool isSelected = currentProjectionTypeString == projectionTypeString[i];
							if (ImGui::Selectable(projectionTypeString[i], isSelected))
							{
								currentProjectionTypeString = projectionTypeString[i];
								component.camera.SetProjectType((Rossweisse::SceneCamera::ProjectionType)i);
							}
							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					if (component.camera.GetProjectType() == Rossweisse::SceneCamera::ProjectionType::Perspective)
					{
						float perspectiveVerticalFov = camera.GetPerspectiveVerticalFOV();
						if (ImGui::DragFloat("FOV", &perspectiveVerticalFov))
							camera.SetPerspectiveVerticalFOV(perspectiveVerticalFov);

						float perspectiveNear = camera.GetPerspectiveNearClip();
						if (ImGui::DragFloat("Near", &perspectiveNear))
							camera.SetPerspectiveNearClip(perspectiveNear);

						float perspectiveFar = camera.GetPerspectiveFarClip();
						if (ImGui::DragFloat("Far", &perspectiveFar))
							camera.SetPerspectiveFarClip(perspectiveFar);
					}

					if (component.camera.GetProjectType() == Rossweisse::SceneCamera::ProjectionType::OrthoGraphics)
					{
						float size = camera.GetOrthographicsSize();
						if (ImGui::DragFloat("Size", &size))
							camera.SetOrthographicsSize(size);

						float farClip = camera.GetOrthographicsFar();
						if (ImGui::DragFloat("FarC", &farClip))
							camera.SetOrthographicsFar(farClip);

						float nearClip = camera.GetOrthographicsNear();
						if (ImGui::DragFloat("Near", &nearClip))
							camera.SetOrthographicsNear(nearClip);

						ImGui::Checkbox("Fied Aspect Ratio", &component.FixedAspectRatio);
					}
				});

		if (entity.HasComponent<Raiden::NodeComponent>()) {
			DrawComponent<Raiden::NodeComponent>("Node", entity, [](Raiden::NodeComponent& component)
				{
					auto& node = component.current;
					ImGui::Text(std::string("Current Node : ").append(std::to_string((uint32_t)node->entity)).c_str());
				});
		}
		if (entity.HasComponent<Raiden::TextureRendererComponent>()) {
			Ref<Object> obj = nullptr;
			if (entity.HasComponent<Raiden::ObjectComponent>()) {
				obj = entity.GetComponent<Raiden::ObjectComponent>().object;
			}
			DrawComponent<Raiden::TextureRendererComponent>("Sprite Renderer", entity, [this, obj](auto& component)
				{
					ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

					//ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
					if (obj && obj->GetObjectClass() == ObjectClass::Mesh) {
						DrawCall* drawcall = dynamic_cast<DrawCall*>(obj.get());
						for (auto& pair : *drawcall) {
							auto texture = pair.second;
							auto name_str = pair.first;
							ImTextureID id = (ImTextureID)(m_Context->GetDefaultImage()->GetRendererID());
							//auto objId = drawcall->getTexture("texture_diffuse");
							//if (objId) {
							uint32_t textureId = texture->GetRendererID();
							if (textureId > 0) {
								id = (ImTextureID)textureId;
							}
							//}
							ImGui::Columns(2); // 
							ImGui::SetColumnWidth(0, 66); // 设置第一列的宽度为200像素
							if (ImGui::ImageButton(id, ImVec2(50.0f, 50.0f)))
							{
								if (Render_Que.find(name_str) == Render_Que.end()) {
									Render_Que[name_str] = std::pair{ ([this,id, name_str,texture,drawcall]() {
										ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
										auto it = Render_Que.find(name_str);
										ImGui::Begin(std::string("Show").append(name_str).c_str(), &it->second.second, ImGuiWindowFlags_NoCollapse);
										ShowImage(texture, drawcall, Computed, [it]() {it->second.second = false; });
										ImGui::Image(id, ImVec2(500.0f, 500.0f));
										ImGui::End();
										}) ,true };
								}
							};
							ImGui::NextColumn(); // 切换到下一列
							//ImGui::Text();
							//ImGui::SameLine();
							//auto drawcall_ptr = CASTUINTPTR(drawcall);
							bool use = drawcall->isUsingTexture(texture);
							if (ImGui::Checkbox(std::string("Name : " + texture->getName()).c_str(), &use)) {
								drawcall->SetUsingTexture(texture, use);
							}
							//ImGui::NextColumn(); // 切换到下一列
							ImGui::Text(std::string("Type : " + texture->getType()).c_str());
							ImGui::SameLine();
							ShowImage(texture, drawcall, Computed);
							ImGui::Columns(1); // 恢复为单列布局
							ImGui::Text(std::string("Manterial : ").append(texture->isTranslucent() ? "半透明" : "不透明").c_str());
							float tiling = drawcall->GetTilingFactor(texture);
							ImGui::DragFloat("Tiling Factor", &tiling, 0.1f, 0.0f, 100.0f);
							drawcall->SetTilingFactor(texture, tiling);
						}
					}
					if (!obj) {
						if (ImGui::Button("Add Texture")) {
							//component.Textures
						}
					}
					if (ImGui::BeginDragDropTarget())
					{
						const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
						if (payload)
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath(path);
							Ref<Bronya::Texture2D> texture = Bronya::Texture2D::Create(texturePath.string());
							if (texture->isBuildState())
								component.Texture = texture;
							else
								Kiana_WARN("Could not load texture {0}", texturePath.filename().string());
						}
						ImGui::EndDragDropTarget();
					}

				});
		}

		if (entity.HasComponent<Raiden::CircleRendererComponent>()) {
			DrawComponent<Raiden::CircleRendererComponent>("Circle", entity, [](auto& component)
				{
					ImGui::DragFloat("##Thickness", &component.Thickness);
					ImGui::DragFloat("##Fade", &component.Fade);
				});
		}
		if (entity.HasComponent<Raiden::SpriteRendererComponent>()) {
			DrawComponent<Raiden::SpriteRendererComponent>("Sprite", entity, [](auto& component)
				{
					ImGui::DragFloat("##border", &component.border);
				});

		}
	}
}