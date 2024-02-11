#pragma once 
#include "k423.h"
#include "WinterSolstice/Core/StructBase.h"
#include "Bounds3.h"
namespace WinterSolstice {

	enum class ObjectClass {
		Object = 0, Model = 1, Mesh = 2, Triangle = 3
	};
	class Object
	{
	public:
		Object();
		virtual ~Object() {};
		virtual void setBounds(glm::vec3 _min, glm::vec3 _max);
		virtual void setBounds(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
		virtual Ref<Bounds3> GetBounds();

		virtual void BuildBVH() = 0;
		virtual bool CheckBVHDown() = 0;
		virtual void BVHDrawBounds() = 0;
		virtual ObjectClass GetObjectClass() { return ObjectClass::Object; }
		virtual float IntersectP(const Ray& ray);
		std::vector<Ref<Object>>& GetOBJ() {
			return objs;
		}
		virtual void DrawBoundsLine() = 0;
		virtual void AddObjects(std::vector<Ref<Object>>& triangles);
		virtual void AddObject(Ref<Object>& obj);

		virtual inline std::vector<Ref<Object>>::iterator begin() { return objs.begin(); }
		virtual inline std::vector<Ref<Object>>::iterator end() { return objs.end(); }
		virtual inline std::vector<Ref<Object>>::reverse_iterator rbegin() { return objs.rbegin(); }
		virtual inline std::vector<Ref<Object>>::reverse_iterator rend() { return objs.rend(); }
		virtual inline std::vector<Ref<Object>>::const_iterator begin() const { return objs.begin(); }
		virtual inline std::vector<Ref<Object>>::const_iterator end() const { return objs.end(); }
		virtual inline std::vector<Ref<Object>>::const_reverse_iterator rbegin() const { return objs.rbegin(); }
		virtual inline std::vector<Ref<Object>>::const_reverse_iterator rend() const { return objs.rend(); }
		virtual void SetName(std::string&& name) { this->name = name; }
		virtual std::string GetName() { return name; }
		virtual Translation GetTranslation() { return trans; }
		virtual void SetTranslation(Translation t);
		virtual glm::vec3 GetZBuffer() { return zBuffer; };
		virtual void SetZBuffer(glm::vec3 set) { zBuffer = set; };
		virtual void DeleteObj(const std::string& name)
		{
			for (auto it = objs.begin(); it != objs.end(); /* no increment here */) {
				if (it->get()->GetName() == name) {
					it = objs.erase(it); // 擦除元素，并返回下一个有效的迭代器
				}
				else {
					++it; // 仅在未擦除时递增迭代器
				}
			}
		}

		static Ref<Object> Create(ObjectClass type);
		static Ref<Object> CreateTriangle(Point pos[3]);
	protected:
		std::vector<Ref<Object>> objs;
		Translation trans;
		std::string name;
		Ref<Bounds3> bounds;
		std::mutex lockPushObjs;
		glm::vec3 zBuffer = glm::vec3{ -std::numeric_limits<float>::infinity(),
									 -std::numeric_limits<float>::infinity(),
									 -std::numeric_limits<float>::infinity() };
	};

}