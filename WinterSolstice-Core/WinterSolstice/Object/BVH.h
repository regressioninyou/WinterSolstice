#pragma once 
#include <glm/glm.hpp>
#include "BVHImp.h"
#include "WinterSolstice/Render/Renderer2D.h"
namespace WinterSolstice {
	class Object;
	struct Bounds3;
	struct Intersection {
		Intersection() {
			happened = false;
			distance = std::numeric_limits<double>::max();
			obj = nullptr;
		}
		bool happened;
		double distance;
		Ref<Object> obj;
	};
	struct BVHBuildNode {
		Ref<Bounds3> bounds;
		Ref<BVHBuildNode> left;
		Ref<BVHBuildNode> right;
		std::vector<Ref<Object>> object;
		float area;
		glm::vec3 translation;  // 保存平移变换的向量
	public:
		int splitAxis = 0, firstPrimOffset = 0, nPrimitives = 0;
		// BVHBuildNode Public Methods
		BVHBuildNode();
	};
	class BVHAccel :public BVH {
	public:
		BVHAccel();

		void buildBVH(std::vector<Ref<Object>> p) override;

		void FlushBVH(std::vector<Ref<Object>> p) override;

		//Bounds3 WorldBound() const;

		Intersection Intersect(const Ray& ray) const override;
		Intersection getIntersection(Ref<BVHBuildNode> node, const Ray& ray)const override;
		bool IntersectP(const Ray& ray) const override;


		void DrawBounds(int depth = -1) override;
		// BVHAccel Private Data
		const int maxPrimsInNode;
		const SplitMethod splitMethod;
	private:
		void DrawCube(Ref<BVHBuildNode> node,int showDepth, int maxdepth = 0);
		Ref<BVHBuildNode> recursiveBuild(std::vector<Ref<Object>> objects, int depth = 0);
		Ref<BVHBuildNode> FlushBuild(Ref<BVHBuildNode> node, std::vector<BVHBuildNode>& nodes);
		Ref<BVHBuildNode> root;
	};

}