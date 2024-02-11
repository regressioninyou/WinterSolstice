#pragma once 
#include "WinterSolstice/Core/Core.h"
#include "Ray.h"
namespace WinterSolstice {
	class Object;
	struct Intersection;
	struct BVHBuildNode;
	class BVH
	{
	public:
		// BVHAccel Public Types
		enum class SplitMethod { NAIVE, SAH };
		virtual Intersection Intersect(const Ray& ray) const = 0;
		virtual Intersection getIntersection(Ref<BVHBuildNode> node, const Ray& ray)const = 0;
		virtual bool IntersectP(const Ray& ray) const = 0;

		virtual void buildBVH(std::vector<Ref<Object>> p) = 0;
		virtual void FlushBVH(std::vector<Ref<Object>> p) = 0;
		virtual void DrawBounds(int depth = -1) = 0;
		static Ref<BVH> Create();
	};
}