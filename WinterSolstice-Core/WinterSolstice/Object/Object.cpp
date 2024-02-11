#include "k423.h"
#include "Object.h"
#include "Model.h"
#include "Mesh.h"
#include "Triangle.h"
#include <glm/gtx/quaternion.hpp>

namespace WinterSolstice {
	void Object::SetTranslation(Translation t)
	{
		if (t.equals(trans))
			return;

		trans = t;
		glm::mat4 rotation = glm::toMat4(glm::quat(t.Rotate));
		glm::mat4 t4 = glm::translate(glm::mat4(1.0f), t.Transform) *
			rotation *
			glm::scale(glm::mat4(1.0f), t.Scale);
		bounds->UpDate(t4);  
		SetZBuffer(bounds->Centroid());
		
	}
	Ref<Object> Object::Create(ObjectClass type)
	{
		switch (type)
		{
		case ObjectClass::Model:
			return CreateRef<Model>();
			break;
		case ObjectClass::Mesh:
			return CreateRef<Mesh>();
			break;
		case ObjectClass::Triangle:
			return CreateRef<Triangle>();
			break;
		}
		return Ref<Object>();
	}
	Ref<Object> Object::CreateTriangle(Point pos[3])
	{
		return CreateRef<Triangle>(pos);
	}
	Object::Object() :trans({ glm::vec3(0.0f),glm::vec3(0.0f), glm::vec3(1.0f) })
	{
		bounds = CreateRef<Bounds3>();
		SetZBuffer(bounds->Centroid());
	}
	void Object::setBounds(glm::vec3 _min, glm::vec3 _max) {
		bounds = CreateRef<Bounds3>(_min, _max);
		SetZBuffer(bounds->Centroid());
	}
	void Object::setBounds(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		bounds = CreateRef<Bounds3>(p1, p2, p3);
		SetZBuffer(bounds->Centroid());
	}
	Ref<Bounds3> Object::GetBounds() { return bounds; }

	float Object::IntersectP(const Ray& ray) {
		glm::vec3 invDir = 1.0f / ray.direction;
		std::array<int, 3> dirIsNeg = {
			invDir.x < 0 ? 1 : 0,
			invDir.y < 0 ? 1 : 0,
			invDir.z < 0 ? 1 : 0
		};
		return bounds->IntersectP(ray, invDir, dirIsNeg);
	}
	void Object::AddObjects(std::vector<Ref<Object>>& triangles) {
		std::unique_lock<std::mutex> lock(lockPushObjs);
		objs.insert(objs.end(), triangles.begin(), triangles.end());
	};
	void Object::AddObject(Ref<Object>& obj) {
		std::unique_lock<std::mutex> lock(lockPushObjs); 
		objs.emplace_back(obj);
	};

}