#include "k423.h"
#include "KdTree.h"

namespace WinterSolstice {


	static float Distance(const glm::vec3 target, const glm::vec3 point)
	{
		return ((target.x - point.x) * (target.x - point.x) +
			(target.y - point.y) * (target.y - point.y) +
			(target.z - point.z) * (target.z - point.z));

	}

	static bool compareX(const glm::vec3 p1, const glm::vec3 p2) {
		return p1.x < p2.x;
	}

	static bool compareY(const glm::vec3 p1, const glm::vec3 p2) {
		return p1.y < p2.y;
	}

	static bool compareZ(const glm::vec3 p1, const glm::vec3 p2) {
		return p1.z < p2.z;
	}

	inline void KdTree::AccelerateBuild(const std::vector<glm::vec3>& points)
	{
		if (points.size() > 100000)
			depthStop = 2;
		else if (points.size() > 10000)
			depthStop = 1;
		else
			depthStop = -1;

		auto func = [&]() {
			auto start = std::chrono::system_clock::now();
			auto startTime = std::chrono::high_resolution_clock::now();
			buildKDTreeT(root, points);
			while (sing >= depthStop * 2)
				std::this_thread::sleep_for(std::chrono::milliseconds(8));
			auto endTime = std::chrono::high_resolution_clock::now();
			auto end = std::chrono::system_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
			std::time_t starttime = std::chrono::system_clock::to_time_t(start);
			std::time_t endtime = std::chrono::system_clock::to_time_t(start);
			//printKDTree(root1);
			std::cout << "Thread Count: " << depthStop * 2 << "  Function execution time2: " << duration.count() << " milliseconds\n";
			kek = true;
			};
		std::thread rootThread(func);
		rootThread.detach();
	}

	void KdTree::Biuld(std::vector<glm::vec3> points)
	{
		buildKDTreeT(root, points);
	}


	inline void KdTree::buildKDTreeT(KDNode*& node, std::vector<glm::vec3> points, int depth) {
		if (points.empty() || points.size() == 0) {
			node = nullptr;
			this->depth = depth > this->depth ? depth : this->depth;
			return;
		}
		int dimension = depth % 3;
		auto comparator = (dimension == 0) ? compareX : ((dimension == 1) ? compareY : compareZ);

		int medianIndex = points.size() >> 1;
		std::nth_element(points.begin(), points.begin() + medianIndex, points.end(), comparator);
		glm::vec3 medianPoint = points[medianIndex];


		node = new KDNode{ medianPoint ,nullptr, nullptr };

		if (points.size() > 50 && depthStop == depth) {
			{
				int dep = depth + 1;
				std::vector<glm::vec3> pos = std::vector<glm::vec3>(points.begin(), points.begin() + medianIndex);
				auto func1 = [&](std::vector<glm::vec3> pos, int depth) {\
					buildKDTree(node->left, pos, depth);
				SingPlus();
					};
				std::thread leftThread(func1, pos, dep);
				leftThread.detach();
			}
			{
				int dep = depth + 1;
				std::vector<glm::vec3> pos = std::vector<glm::vec3>(points.begin() + medianIndex + 1, points.end());
				auto func2 = [&](std::vector<glm::vec3> pos, int depth) {\
					buildKDTree(node->right, pos, depth);
				SingPlus();
					};
				std::thread rightThread(func2, pos, dep);

				rightThread.detach();
			}
		}
		else {
			buildKDTreeT(node->left, std::vector<glm::vec3>(points.begin(), points.begin() + medianIndex), depth + 1);
			buildKDTreeT(node->right, std::vector<glm::vec3>(points.begin() + medianIndex + 1, points.end()), depth + 1);
		}
		return;
	}

	inline void KdTree::buildKDTree(KDNode*& node, std::vector<glm::vec3> points, int depth) {
		if (points.empty() || points.size() == 0) {
			this->depth = depth > this->depth ? depth : this->depth;
			node = nullptr;
			return;
		}

		int dimension = depth % 3;
		auto comparator = (dimension == 0) ? compareX : ((dimension == 1) ? compareY : compareZ);

		int medianIndex = points.size() >> 1;
		std::nth_element(points.begin(), points.begin() + medianIndex, points.end(), comparator);
		glm::vec3 medianPoint = points[medianIndex];


		node = new KDNode{ medianPoint ,nullptr, nullptr };

		//points.erase(points.begin() + medianIndex);
		buildKDTree(node->left, std::vector<glm::vec3>(points.begin(), points.begin() + medianIndex), depth + 1);
		buildKDTree(node->right, std::vector<glm::vec3>(points.begin() + medianIndex + 1, points.end()), depth + 1);
	}

	void KdTree::insert(glm::vec3 point)
	{
		setCount++;
		insertKDNode(root, point);
	}

	inline KDNode* KdTree::insertKDNode(KDNode* root, const glm::vec3& newPoint, int depth) {
		if (root == nullptr) {
			return new KDNode{ newPoint, nullptr, nullptr };
		}

		int dimension = depth % 3;
		auto comparator = (dimension == 0) ? compareX : ((dimension == 1) ? compareY : compareZ);

		if (comparator(newPoint, root->point)) {
			root->left = insertKDNode(root->left, newPoint, depth + 1);
		}
		else {
			root->right = insertKDNode(root->right, newPoint, depth + 1);
		}

		return root;
	}
	void KdTree::query(glm::vec3 point)
	{
		float bestDistance = 0.0f;
		KDTreeSearch(root,point,0, bestDistance);
	}
	inline KDNode* KdTree::KDTreeSearch(KDNode* root, const glm::vec3& target, int depth, float& bestDistance) {
		if (root == nullptr) {
			return nullptr;
		}
		KDNode* bestNode;
		int dimension = depth % 3;
		auto comparator = (dimension == 0) ? compareX : ((dimension == 1) ? compareY : compareZ);
		//判断下一个节点的索引值是依赖x,y，z来进行搜索
		KDNode* nextBranch = (comparator(target, root->point)) ? root->left : root->right;
		//获取另一边的子节点
		KDNode* otherBranch = (nextBranch == root->left) ? root->right : root->left;

		bestDistance = Distance(root->point,target);
		//在下一个节点进行搜索
		KDNode* nextBest = KDTreeSearch(nextBranch, target, depth + 1, bestDistance);

		if (nextBest) {
			float nextDistance = Distance(target,nextBest->point);

			if (nextDistance < bestDistance) {
				bestDistance = nextDistance;
				bestNode = nextBest;
			}
		}

		float axisDistance = 0.0f;
		switch (dimension) {
			case 0: axisDistance = target.x - root->point.x; break;
			case 1: axisDistance = target.y - root->point.y; break;
			case 2: axisDistance = target.z - root->point.z; break;
		}

		// 判断是否需要在另一边搜索
		if (axisDistance * axisDistance < bestDistance) {
			KDNode* otherBest = KDTreeSearch(otherBranch, target, depth + 1, bestDistance);
			if (otherBest) {
				float otherDistance = std::sqrt((target.x - otherBest->point.x) * (target.x - otherBest->point.x) +
					(target.y - otherBest->point.y) * (target.y - otherBest->point.y) +
					(target.z - otherBest->point.z) * (target.z - otherBest->point.z));

				if (otherDistance < bestDistance) {
					bestDistance = otherDistance;
					bestNode = otherBest;
				}
			}
		}

		return bestNode;
	}

	inline void KdTree::deleteKDTree(KDNode* root) {
		if (root) {
			deleteKDTree(root->left);
			deleteKDTree(root->right);
			delete root;
		}
	}

}
