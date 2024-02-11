#pragma once
#include "WinterSolstice/Core/Core.h"
#include <vector>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <thread>
#include <functional>
#include <ctime>
#include <chrono>
#include <mutex>
namespace WinterSolstice {

	struct KDNode {
		glm::vec3 point;
		KDNode* left = nullptr;
		KDNode* right = nullptr;
	};
	class KdTree
	{
	public:
		KdTree() = default;
		~KdTree() { deleteKDTree(root); }
		void AccelerateBuild(const std::vector<glm::vec3>& points);
		void Biuld(std::vector<glm::vec3> points);
		void insert(glm::vec3 point);
		void query(glm::vec3 point);
		void find_nearest_point();

		int depth = 0;
		bool GetKey() { return kek; }
		KDNode* getRoot() { return root; }
	private:
		int depthStop = 1;
		bool kek = false;
		std::mutex singMutex;
		std::mutex mutex;
		int sing = 0;
		KDNode* root;

		uint32_t setCount = 0;
	private:
		void buildKDTreeT(KDNode*& node, std::vector<glm::vec3> points, int depth = 0);
		void buildKDTree(KDNode*& node, std::vector<glm::vec3> points, int depth = 0);
		KDNode* insertKDNode(KDNode* root, const glm::vec3& newPoint, int depth = 0);
		KDNode* KDTreeSearch(KDNode* root, const glm::vec3& target, int depth, float& bestDistance);
		void deleteKDTree(KDNode* root);
		void SingPlus() {
			std::unique_lock<std::mutex> lock(singMutex);  // 锁定互斥锁
			sing++;
		}
	};
}