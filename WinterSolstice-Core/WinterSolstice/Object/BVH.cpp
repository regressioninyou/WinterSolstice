#include "k423.h"
#include "BVH.h"
#include "Bounds3.h"
#include "Object.h"
#include <WinterSolstice/Utils/ThreadPool.h>
#include <execution>
namespace WinterSolstice {
	BVHAccel::BVHAccel() :root(nullptr), splitMethod(BVH::SplitMethod::NAIVE), maxPrimsInNode(1024) {}
	void BVHAccel::buildBVH(std::vector<Ref<Object>> p)
	{
		root = recursiveBuild(p);
	}
	void BVHAccel::FlushBVH(std::vector<Ref<Object>> p) {
		root = recursiveBuild(p);
	}
	Intersection BVHAccel::Intersect(const Ray& ray) const {
		Intersection isect;
		if (!root)
			return isect;
		isect = BVHAccel::getIntersection(root, ray);
		return isect;
	}
	Intersection BVHAccel::getIntersection(Ref<BVHBuildNode> node, const Ray& ray) const {
		Intersection isect;
		if (!root)
			return isect;
		glm::vec3 localOrigin = ray.origin - root->bounds->pMin;
		glm::vec3 localDirection = ray.direction;
		float tMin = 0.0f;
		float tMax = std::numeric_limits<float>::infinity();
		bool intersection = false;
		for (int i = 0; i < 3; ++i) {
			if (std::abs(localDirection[i]) < std::numeric_limits<float>::epsilon()) {
				// 射线平行于轴，如果起点在包围盒之外，那么射线与包围盒不相交
				if (localOrigin[i] < 0.0f || localOrigin[i] > node->bounds->pMax[i] - node->bounds->pMin[i]) {
					break;
				}
			}
			else {
				// 计算与轴平行的包围盒的相交范围
				float t0 = (node->bounds->pMin[i] - localOrigin[i]) / localDirection[i];
				float t1 = (node->bounds->pMax[i] - localOrigin[i]) / localDirection[i];

				// 确保 t0 是较小的值，t1 是较大的值
				if (t0 > t1) {
					std::swap(t0, t1);
				}

				// 更新 tMin 和 tMax
				tMin = std::max(t0, tMin);
				tMax = std::min(t1, tMax);

				// 如果 tMin 大于 tMax，表示射线与包围盒在这个轴上没有相交
				if (tMin > tMax) {
					break;
				}
			}
		}
		if (tMin <= tMax)
		{
			if (node->left) {
				Intersection leftIsect = getIntersection(node->left, ray);
				if (leftIsect.happened && leftIsect.distance < isect.distance) {
					isect = leftIsect;
				}
			}
			if (node->right) {
				Intersection rightIsect = getIntersection(node->right, ray);
				if (rightIsect.happened && rightIsect.distance < isect.distance) {
					isect = rightIsect;
				}
			}
		}
		return isect;
	}
	bool BVHAccel::IntersectP(const Ray& ray) const {
		glm::vec3 localOrigin = ray.origin - root->bounds->pMin;
		glm::vec3 localDirection = ray.direction;
		float tMin = 0.0f;
		float tMax = std::numeric_limits<float>::infinity();
		for (int i = 0; i < 3; ++i) {
			if (std::abs(localDirection[i]) < std::numeric_limits<float>::epsilon()) {
				// 射线平行于轴，如果起点在包围盒之外，那么射线与包围盒不相交
				if (localOrigin[i] < 0.0f || localOrigin[i] > root->bounds->pMax[i] - root->bounds->pMin[i]) {
					return false;
				}
			}
			else {
				// 计算与轴平行的包围盒的相交范围
				float t0 = (root->bounds->pMin[i] - localOrigin[i]) / localDirection[i];
				float t1 = (root->bounds->pMax[i] - localOrigin[i]) / localDirection[i];

				// 确保 t0 是较小的值，t1 是较大的值
				if (t0 > t1) {
					std::swap(t0, t1);
				}

				// 更新 tMin 和 tMax
				tMin = std::max(t0, tMin);
				tMax = std::min(t1, tMax);

				// 如果 tMin 大于 tMax，表示射线与包围盒在这个轴上没有相交
				if (tMin > tMax) {
					return false;
				}
			}
		}
		return tMin <= tMax;
	}
	void BVHAccel::DrawBounds(int depth)
	{
		DrawCube(root, depth);
	}
	void BVHAccel::DrawCube(Ref<BVHBuildNode> node, int showDepth, int maxdepth) {
		if (!node || maxdepth == 5)
		{
			return;
		}
		auto draw = [&]() {
			glm::vec3 pMin = node->bounds->pMin;
			glm::vec3 pMax = node->bounds->pMax;
			glm::vec3 point[8];
			// 生成矩形的八个顶点
			point[0] = { pMin.x, pMin.y, pMin.z }; // 左下角
			point[1] = { pMax.x, pMin.y, pMin.z }; // 右下角
			point[2] = { pMax.x, pMax.y, pMin.z }; // 右上角
			point[3] = { pMin.x, pMax.y, pMin.z }; // 左上角

			point[4] = { pMin.x, pMin.y, pMax.z }; // 左下角的上方
			point[5] = { pMax.x, pMin.y, pMax.z }; // 右下角的上方
			point[6] = { pMax.x, pMax.y, pMax.z }; // 右上角的上方
			point[7] = { pMin.x, pMax.y, pMax.z }; // 左上角的上方
			// 依次绘制每一条线
			/*for (int i = 0; i < 7; i += 2) {
			Renderer2D::DrawLine(point[i], point[i + 1], { 0.1f,0.6f,0.3f,1.0f });
			}*/
			// 绘制长方体的八条边
			// 绘制底面
			Bronya::Renderer2D::DrawLine(point[0], point[1], { 0.1f,0.6f,0.3f,1.0f });
			Bronya::Renderer2D::DrawLine(point[1], point[2], { 0.1f,0.6f,0.3f,1.0f });
			Bronya::Renderer2D::DrawLine(point[2], point[3], { 0.1f,0.6f,0.3f,1.0f });
			Bronya::Renderer2D::DrawLine(point[3], point[0], { 0.1f,0.6f,0.3f,1.0f });

			// 顶面
			Bronya::Renderer2D::DrawLine(point[4], point[5], { 0.1f,0.6f,0.3f,1.0f });
			Bronya::Renderer2D::DrawLine(point[5], point[6], { 0.1f,0.6f,0.3f,1.0f });
			Bronya::Renderer2D::DrawLine(point[6], point[7], { 0.1f,0.6f,0.3f,1.0f });
			Bronya::Renderer2D::DrawLine(point[7], point[4], { 0.1f,0.6f,0.3f,1.0f });

			// 连接底面和顶面的边
			Bronya::Renderer2D::DrawLine(point[0], point[4], { 0.1f,0.6f,0.3f,1.0f });
			Bronya::Renderer2D::DrawLine(point[1], point[5], { 0.1f,0.6f,0.3f,1.0f });
			Bronya::Renderer2D::DrawLine(point[2], point[6], { 0.1f,0.6f,0.3f,1.0f });
			Bronya::Renderer2D::DrawLine(point[3], point[7], { 0.1f,0.6f,0.3f,1.0f });

			for (auto obj : node->object) {
				if (obj->CheckBVHDown())
					obj->BVHDrawBounds();
			}
			};
		if (showDepth == -1)
		{
			draw();
		}
		else
			if (showDepth > -1 && maxdepth == showDepth)
			{
				draw();
				return;
			}

		DrawCube(node->left, showDepth, maxdepth + 1);
		DrawCube(node->right, showDepth, maxdepth + 1);
	}

	Ref<BVHBuildNode> BVHAccel::recursiveBuild(std::vector<Ref<Object>> objects, int depth)
	{
		Ref<BVHBuildNode> node = CreateRef<BVHBuildNode>();
		if (objects.size() == 1 || depth == 0)
		{
			Ref<Bounds3> bounds = CreateRef<Bounds3>();
			//先讲所有对象遍历创建一个大正方形包围盒
			for (int i = 0; i < objects.size(); ++i)
				bounds = Union(*bounds, *objects[i]->GetBounds());
			node->bounds = bounds;
			node->object = objects;
			//std::for_each(std::execution::par, node->object.begin(), node->object.end(), [&](Ref<Object> obj) {
			//	obj->BuildBVH();
			//	});
			node->left = nullptr;
			node->right = nullptr;
			return node;
		}
		else if (objects.size() == 2)
		{
			node->left = recursiveBuild(std::vector{ objects[0] }, depth + 1);
			node->right = recursiveBuild(std::vector{ objects[1] }, depth + 1);
			node->bounds = Union(*node->left->bounds, *node->right->bounds);
			return node;
		}
		else {
			Ref<Bounds3> centroidBounds = CreateRef<Bounds3>();
			for (int i = 0; i < objects.size(); ++i)
				centroidBounds = Union(*centroidBounds, objects[i]->GetBounds()->Centroid());
			int dim = centroidBounds->maxExtent();
			switch (dim) {
			case 0:
				std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
					return f1->GetBounds()->Centroid().x <
						f2->GetBounds()->Centroid().x;
					});
				break;
			case 1:
				std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
					return f1->GetBounds()->Centroid().y <
						f2->GetBounds()->Centroid().y;
					});
				break;
			case 2:
				std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
					return f1->GetBounds()->Centroid().z <
						f2->GetBounds()->Centroid().z;
					});
				break;
			}

			auto beginning = objects.begin();
			auto middling = objects.begin() + (objects.size() / 2);
			auto ending = objects.end();

			auto leftshapes = std::vector<Ref<Object>>(beginning, middling);
			auto rightshapes = std::vector<Ref<Object>>(middling, ending);

			assert(objects.size() == (leftshapes.size() + rightshapes.size()));

			//if (leftshapes.size() > 1000)
			//{
			//	ThreadPool threadpool(2);
			//	auto leftFuture = threadpool.enqueue([&]() {
			//		return recursiveBuild(leftshapes,depth +1);
			//		});

			//	auto rightFuture = threadpool.enqueue([&]() {
			//		return recursiveBuild(rightshapes, depth + 1);
			//		});

			//	// 获取左右子树的结果
			//	node->left = leftFuture.get();
			//	node->right = rightFuture.get();
			//}
			//else
			//{
				node->left = recursiveBuild(leftshapes, depth + 1);
				node->right = recursiveBuild(rightshapes, depth + 1);
			//}

			node->bounds = Union(node->left->bounds, node->right->bounds);
		}
		return node;
	}

	Ref<BVHBuildNode> BVHAccel::FlushBuild(Ref<BVHBuildNode> node, std::vector<BVHBuildNode>& nodes)
	{
		return nullptr;
	}

	// BVHBuildNode Public Methods

	BVHBuildNode::BVHBuildNode() {
		bounds = CreateRef<Bounds3>();
		left = nullptr;
		right = nullptr;
	}
}