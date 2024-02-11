#pragma once 
#include "k423.h"
#include <map>
#include "Animation.h"
namespace WinterSolstice {
	struct BoneNode
	{
		std::string name;
		uint32_t index;
		glm::mat4 OffsetMatrix;
		glm::mat4 BaseTransform;
		glm::mat4 FinalTransformation;
		Ref<BoneNode> parent;
		std::vector<Ref<BoneNode>> child;
		BoneNode() = default;
		BoneNode(const glm::mat4& Offset)
		{
			OffsetMatrix = Offset;
			FinalTransformation = glm::mat4(1.0f);
			BaseTransform = glm::mat4(1.0f);
		}
		BoneNode(const glm::mat4& Offset, std::string name) :name(name)
		{
			OffsetMatrix = Offset;
			FinalTransformation = glm::mat4(1.0f);
			BaseTransform = glm::mat4(1.0f);
		}
		BoneNode(const glm::mat4& Offset, std::string name, uint32_t idx) :name(name), index(idx)
		{
			OffsetMatrix = Offset;
			FinalTransformation = glm::mat4(1.0f);
			BaseTransform = glm::mat4(1.0f);
		}
	};
	struct Bone
	{
		void SetRoot(Ref<BoneNode>& BoneRoot)
		{
			root = BoneRoot;
		};

		void AddAnimation(Ref<Animation>& animation)
		{
			if (name_Animation_map.find(animation->GetName()) != name_Animation_map.end())
				name_Animation_map.insert({ animation->GetName(),animation });
			else
				name_Animation_map[animation->GetName()] = animation;
		}

		void Update(Kiana::Timestep ts, const std::string& AnimationName)
		{
			if (name_Animation_map.find(AnimationName) != name_Animation_map.end())
			{
				name_Animation_map[AnimationName]->Update(ts);
				for (auto& child : root->child) {
					glm::mat4 result = glm::mat4(1.0f);
					EachBoneKeyFrameResultMat4(child, ts, AnimationName, result);
				}
			}
			else {
			}
		}
		void BaseUpdate() {
			for (auto& child : root->child) {
				glm::mat4 result = glm::mat4(1.0f);
				BaseEachBoneKeyFrameResultMat4(child, result);
			}
		}
		void EachBoneKeyFrameResultMat4(Ref<BoneNode>& node,Kiana::Timestep ts, const std::string& AnimationName, glm::mat4& ParentTransform)
		{
			auto animation = name_Animation_map[AnimationName];
			auto mat4 = animation->GetKeyFrame(node->name);
			auto animTrans = node->BaseTransform;
			if (mat4 != glm::mat4(1.0f))
				animTrans = mat4;
			glm::mat4 transform = ParentTransform * animTrans;
			auto trans = Bone_Offset_ZERO * transform * node->OffsetMatrix;
			CurrentAnimationTransformMap[node->name] = trans;
			IDCurrentAnimationTransformMap[node->index] = trans;
			node->FinalTransformation = trans;
			for (auto& child : node->child) {
				EachBoneKeyFrameResultMat4(child, ts, AnimationName, transform);
			}
		}
		void BaseEachBoneKeyFrameResultMat4(Ref<BoneNode>& node, glm::mat4& ParentTransform)
		{
			auto animTrans = node->BaseTransform;
			glm::mat4 transform = ParentTransform * animTrans;
			auto trans = transform * node->OffsetMatrix;
			CurrentAnimationTransformMap[node->name] = trans;
			IDCurrentAnimationTransformMap[node->index] = trans;
			for (auto& child : node->child) {
				BaseEachBoneKeyFrameResultMat4(child, transform);
			}
		}

		std::unordered_map<std::string, Ref<Animation>> name_Animation_map;
		std::unordered_map<std::string, uint32_t> indexBoneMap;
		std::unordered_map<std::string, glm::mat4> CurrentAnimationTransformMap;
		std::map<uint32_t, glm::mat4> IDCurrentAnimationTransformMap;
		glm::mat4 Bone_Offset_ZERO;
		Ref<BoneNode> root;
	};
}