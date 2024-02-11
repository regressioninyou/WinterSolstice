#include "k423.h"
#include "assimp/XmlParser.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Components.h"
namespace WinterSolstice {
	namespace Raiden {
		SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
			:m_Scene(scene)
		{
		}
		static void SerializeEntity(Entity entity, pugi::xml_node& node) {
			if (entity.HasComponent<TagComponent>())
			{
				auto& tag = entity.GetComponent<TagComponent>().Tag;
				auto tagNode = node.append_child("Tag");
				tagNode.append_attribute("name") = tag.c_str();
			}

			if (entity.HasComponent<TransformComponent>())
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				auto child = node.append_child("Transform");

				auto translation = child.append_child("Translation");
				translation.append_attribute("type") = "Float3";
				translation.append_attribute("x") = transform.Translation.x;
				translation.append_attribute("y") = transform.Translation.y;
				translation.append_attribute("z") = transform.Translation.z;

				auto rotation = child.append_child("Rotation");
				rotation.append_attribute("type") = "Float3";
				rotation.append_attribute("x") = transform.Rotation.x;
				rotation.append_attribute("y") = transform.Rotation.y;
				rotation.append_attribute("z") = transform.Rotation.z;

				auto scale = child.append_child("Scale");
				scale.append_attribute("type") = "Float3";
				scale.append_attribute("x") = transform.Scale.x;
				scale.append_attribute("y") = transform.Scale.y;
				scale.append_attribute("z") = transform.Scale.z;
			}

			if (entity.HasComponent<SpriteRendererComponent>())
			{
				auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();
				auto child = node.append_child("SpriteRendererComponent");

				auto color = child.append_child("Color");
				color.append_attribute("type") = "Float3";
				color.append_attribute("r") = spriteRenderer.Color.r;
				color.append_attribute("g") = spriteRenderer.Color.g;
				color.append_attribute("b") = spriteRenderer.Color.b;
				color.append_attribute("a") = spriteRenderer.Color.a;
			}

			if (entity.HasComponent<CameraComponent>())
			{
				auto& cameraComponent = entity.GetComponent<CameraComponent>();
				auto child = node.append_child("Camera");
				auto primary = child.append_child("Primary");
				primary.append_attribute("type") = "bool";
				primary.append_attribute("value") = cameraComponent.Primary;

				auto fixe = child.append_child("FixedAspectRatio");
				fixe.append_attribute("type") = "bool";
				fixe.append_attribute("value") = cameraComponent.FixedAspectRatio;


				auto camera = child.append_child("SceneCamera");
				camera.append_attribute("ProjectionType") = (int)cameraComponent.camera.GetProjectType();
				camera.append_attribute("OrthographicSize") = cameraComponent.camera.GetOrthographicsSize();
				camera.append_attribute("OrthographicNear") = cameraComponent.camera.GetOrthographicsNear();
				camera.append_attribute("OrthographicFar") = cameraComponent.camera.GetOrthographicsFar();
				camera.append_attribute("PerspectiveFOV") = cameraComponent.camera.GetPerspectiveVerticalFOV();
				camera.append_attribute("PerspectiveNear") = cameraComponent.camera.GetOrthographicsNear();
				camera.append_attribute("PerspectiveFar") = cameraComponent.camera.GetPerspectiveFarClip();
			}
		}

		void SceneSerializer::Serialize(const std::string& filepath)
		{
			pugi::xml_document doc;
			doc.reset();
			pugi::xml_node node = doc.append_child("Scene");
			m_Scene->m_Registry.each([&](auto entityID)
				{
					auto et = node.append_child("Entity");
					Entity entity = { entityID,m_Scene.get() };
					if (!entity)
						return;
					SerializeEntity(entity, et);
				});

			doc.save_file(filepath.c_str());
		}
		void SceneSerializer::SerializeRuntime(const std::string& filepath)
		{
		}
		bool SceneSerializer::Deserialize(const std::string& filepath)
		{
			pugi::xml_document doc;
			doc.reset();
			auto result = doc.load_file(filepath.c_str());
			if (result.status == pugi::status_ok)
			{
				auto scene = doc.child("Scene");
				for (auto entity : scene)
				{
					Entity enti = m_Scene->CreateEntity();
					for (auto component : entity)
					{
						std::string name = component.name();
						if (name == "Tag") {
							auto& tag = enti.AddComponent<TagComponent>();
							tag.Tag = component.attribute("name").value();
						}
						else if (name == "Transform")
						{
							auto& transform = enti.AddComponent<TransformComponent>();
							auto Translation = component.child("Translation");
							glm::vec3 ts = glm::vec3{ Translation.attribute("x").as_float(),Translation.attribute("y").as_float(),Translation.attribute("z").as_float() };
							transform.Translation = ts;
							auto Rotation = component.child("Rotation");
							transform.Rotation = glm::vec3{ Rotation.attribute("x").as_float(),Rotation.attribute("y").as_float(),Rotation.attribute("z").as_float() };
							auto Scale = component.child("Scale");
							transform.Scale = glm::vec3{ Scale.attribute("x").as_float(),Scale.attribute("y").as_float(),Scale.attribute("z").as_float() };
						}
						else if (name == "Camera")
						{
							auto& camera = enti.AddComponent<CameraComponent>();
							camera.Primary = component.child("Primary").attribute("value").as_bool();
							camera.FixedAspectRatio = component.child("FixedAspectRatio").attribute("value").as_bool();
							auto SceneCamera = component.child("SceneCamera");
							
							camera.camera.SetProjectType((Rossweisse::SceneCamera::ProjectionType)SceneCamera.attribute("ProjectionType").as_int());
							camera.camera.SetOrthographicsSize(SceneCamera.attribute("OrthographicSize").as_float());
							camera.camera.SetOrthographicsFar(SceneCamera.attribute("OrthographicFar").as_float());
							camera.camera.SetOrthographicsNear(SceneCamera.attribute("OrthographicNear").as_float());

							camera.camera.SetPerspectiveFarClip(SceneCamera.attribute("PerspectiveFar").as_float());
							camera.camera.SetPerspectiveNearClip(SceneCamera.attribute("PerspectiveNear").as_float());
							camera.camera.SetPerspectiveVerticalFOV(SceneCamera.attribute("PerspectiveFOV").as_float());
						}
						else if (name == "SpriteRendererComponent")
						{
							auto& spriteRenderer = enti.AddComponent<SpriteRendererComponent>();
							auto color = component.child("Color");
							spriteRenderer.Color = glm::vec4{ color.attribute("r").as_float(),color.attribute("g").as_float() ,color.attribute("b").as_float() ,color.attribute("a").as_float() };
						}
					}
				}
				return true;
			}
			Kiana_WARN(result.description());
			return false;
		}
		bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
		{
			//Kiana_CORE_ASSERT(false);
			//balabala

			return false;
		}
	}
}