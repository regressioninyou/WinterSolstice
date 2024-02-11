#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "WinterSolstice/Camera/Camera.h"
#include <WinterSolstice/Camera/SceneCamera.h>
#include <WinterSolstice/Render/Texture.h>
#include <WinterSolstice/Render/Shader.h>
#include "WinterSolstice/Object/Object.h"
#include "ScritableEntity.h"
#include <glm/gtx/quaternion.hpp>
#include <WinterSolstice/Animation/Animation.h>
#include <WinterSolstice/Light/Light.h>

namespace WinterSolstice {
	namespace Raiden {

		struct TagComponent
		{
			std::string Tag;

			TagComponent() = default;
			TagComponent(const TagComponent&) = default;
			TagComponent(const std::string& tag)
				: Tag(tag) {}
		};

		struct TransformComponent
		{
			glm::vec3 Translation = glm::vec3(0.0f);
			glm::vec3 Rotation = glm::vec3(0.0f);
			glm::vec3 Scale = glm::vec3(1.0f);
			TransformComponent operator+(const TransformComponent& other) {
				TransformComponent t;
				t.Translation = Translation + other.Translation;
				t.Rotation = Rotation + other.Rotation;
				t.Scale = Scale * other.Scale;
				return t;
			}
			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;
			TransformComponent(const glm::vec3& transform)
				: Translation(transform) {}
			glm::mat4 GetTransform() const {
				glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));

				return  glm::translate(glm::mat4(1.0f), Translation) *
					rotation *
					glm::scale(glm::mat4(1.0f), Scale);
			}

			operator glm::mat4() const {
				return GetTransform();
			}
		};
		struct CircleRendererComponent
		{
			glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
			float Thickness = 1.0f;
			float Fade = 0.005f;

			CircleRendererComponent() = default;
			CircleRendererComponent(const CircleRendererComponent&) = default;
		};
		struct NodeComponent
		{
			Ref<Scene::ListNode> current = nullptr;

			NodeComponent() = default;
			NodeComponent(const NodeComponent&) = default;
			NodeComponent(Ref<Scene::ListNode> c, Scene* scene)
				:current(c) {};
		};
		struct ObjectComponent
		{
			Ref<Object> object;

			ObjectComponent() = default;
			ObjectComponent(const ObjectComponent&) = default;
			ObjectComponent(const Ref<Object> obj)
				: object(obj) {}
		};
		struct MaterialComponent
		{
			Ref<Bronya::ShaderLibrary> material;

			MaterialComponent() = default;
			MaterialComponent(const MaterialComponent&) = default;
			MaterialComponent(const Ref<Bronya::ShaderLibrary> shader)
				: material(shader) {}
		};
		struct AnimationComponent {
			Ref<Animation> Animate;

			AnimationComponent() = default;
			AnimationComponent(const AnimationComponent&) = default;
			AnimationComponent(const Ref<Animation> a)
				: Animate(a) {}
		};

		struct SpriteRendererComponent
		{
			glm::vec4 Color = glm::vec4(1.0f);
			Ref<Bronya::Texture2D> Texture;
			std::unordered_map<std::string, Ref<Bronya::Texture2D>> Textures;
			float TilingFactor = 1.0f;

			SpriteRendererComponent() = default;
			SpriteRendererComponent(const SpriteRendererComponent&) = default;
			SpriteRendererComponent(const glm::vec4& color)
				: Color(color) {}

			operator glm::vec4& () { return Color; }
			operator const glm::vec4& () const { return Color; }
		};
		struct AwalysShineComponent {
			Ref<Kallen::Light> AwalysShine;
			AwalysShineComponent() = default;
			AwalysShineComponent(const AwalysShineComponent&) = default;
			AwalysShineComponent(Ref<Kallen::Light> shine)
				: AwalysShine(shine) {}
		};
		struct CameraComponent
		{
			Rossweisse::SceneCamera camera;
			//是否激活
			bool Primary = true;
			//控制旋转
			bool FixedAspectRatio = false;

			CameraComponent() = default;
			CameraComponent(const CameraComponent&) = default;
			CameraComponent(const glm::mat4& projection)
				: camera(projection) {}
		};
		struct ScriptComponent
		{
			std::string ClassName;

			ScriptComponent() = default;
			ScriptComponent(const ScriptComponent&) = default;
		};
		struct NativeScriptComponent
		{
			ScritableEntity* Instance = nullptr;


			ScritableEntity* (*InstantiateScript)();
			void (*DestoryScript)(NativeScriptComponent*);

			template<typename T>
			void Bind()
			{
				InstantiateScript = []() {return static_cast<ScritableEntity*>(new T()); };
				DestoryScript = [](NativeScriptComponent* nsc) {
					delete nsc->Instance;
					nsc->Instance = nullptr;
					};
			}
		};
	}
}
