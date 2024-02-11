#pragma once 
#include "k423.h"
#include "WinterSolstice/Core/StructBase.h"
#include "Ray.h"
namespace WinterSolstice {
	class Bounds3
	{
	public:
		glm::vec3 pMin, pMax;
		Bounds3();;
		Bounds3(const glm::vec3 p);
		Bounds3(const glm::vec3 p1, const glm::vec3 p2);
		Bounds3(const glm::vec3 p1, const glm::vec3 p2, const glm::vec3 p3);
		glm::vec3 Diagonal() const;
		int maxExtent() const;

		double SurfaceArea() const;


		glm::vec3 Centroid();
		//合并包围盒，交集合并
		Bounds3 Intersect(const Bounds3& b);

		//获取点p在包围盒内相对位置
		glm::vec3 Offset(const glm::vec3& p) const;

		//检测包围盒是否重叠
		bool Overlaps(const Bounds3& b1, const Bounds3& b2);

		bool Inside(const glm::vec3& p, const Bounds3& b);

		void UpDate(glm::mat4 trans);
		inline const glm::vec3& operator[](int i) const;

		bool IntersectP(const Ray& ray, const glm::vec3& invDir,
			const std::array<int, 3>& dirisNeg) const;
	};
	Ref<Bounds3> Union(const Ref<Bounds3> b1, const Ref<Bounds3> b2);
	Ref<Bounds3> Union(const Bounds3& b1, const Bounds3& b2);
	Ref<Bounds3> Union(const Bounds3& b, const glm::vec3& p);
}