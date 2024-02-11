#include "k423.h"
#include "WinterSolstice/Animation/AnimationBone.h"
#include "WinterSolstice/Core/StructBase.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace WinterSolstice {
	AnimationBone::AnimationBone(const std::string& n) :name(n)
	{
	}
	AnimationBone::~AnimationBone()
	{
	}
	void AnimationBone::SetModelKeyFrame(const std::string& BoneName, Ref<ModelKeyframeAnimation> anim)
	{
		if (Bonde_name_to_index_map.find(BoneName) == Bonde_name_to_index_map.end()) {
			Bonde_name_to_index_map[BoneName] = Bonde_name_to_index_map.size();
		}
		uint32_t id = Bonde_name_to_index_map[BoneName];
		iKeyFrame[id] = anim;
	}

	void AnimationBone::AddModelKeyFrame(Ref<ModelKeyframeAnimation> anim)
	{
		Bonde_name_to_index_map[anim->name] = Bonde_name_to_index_map.size();
		uint32_t id = Bonde_name_to_index_map[anim->name];
		iKeyFrame[id] = anim;
	}
	uint32_t AnimationBone::GetBoneIndex(const std::string& BoneName)
	{
		return Bonde_name_to_index_map[BoneName];
	}
	glm::mat4 AnimationBone::GetKeyFrame(const std::string& BoneName)
	{
		//if (preTime > AnimationTime && !loop) return glm::mat4(1.0f);

		uint32_t ID = Bonde_name_to_index_map[BoneName];

		auto BoneKey = iKeyFrame[ID];
		if (!BoneKey) 
		{
			return glm::mat4(1.0f);
		}
		auto Translation = CalcInterpolatedPosition(current, BoneKey) * 0.001f;
		auto rotate = CalcInterpolatedRotation(current, BoneKey);
		auto Scale = CalcInterpolatedScaling(current, BoneKey) * 0.001f;

		glm::mat4 rotation = (glm::toMat4(glm::normalize(rotate)));
		return  
			glm::translate(glm::mat4(1.0f), Translation) *
			rotation *
			glm::scale(glm::mat4(1.0f), Scale);
	}
	void AnimationBone::SetLoop(bool loop)
	{
		this->loop = loop;
	}
	void AnimationBone::SetAnimationLoopTime(double time)
	{
		AnimationTime = time;
	}
	uint32_t AnimationBone::getBoueId(const aiBone* bone)
	{
		std::string name = std::string(bone->mName.C_Str());
		if (Bonde_name_to_index_map.find(name) == Bonde_name_to_index_map.end())
		{
			Bonde_name_to_index_map[name] = Bonde_name_to_index_map.size();
		}
		return Bonde_name_to_index_map[name];
	}
	std::string AnimationBone::GetName()
	{
		return name;
	}
	void AnimationBone::SetName(const std::string& name)
	{
		this->name = name;
	}
	void AnimationBone::SetInterpolationFunc(Interpolation type)
	{
		this->type = type;
	}
	glm::vec3 AnimationBone::CalcInterpolatedScaling(double ts, Ref<ModelKeyframeAnimation>& BoneKey)
	{
		if (type == Interpolation::Linear) {
			uint32_t index = FindScaleIndex(ts, BoneKey);
			uint32_t nextIndex = index + 1;
			if (nextIndex >= BoneKey->ScaleKeyFrame.size()) return BoneKey->ScaleKeyFrame[index].ScaleKey;

			double t1 = BoneKey->ScaleKeyFrame[index].Time;
			double t2 = BoneKey->ScaleKeyFrame[nextIndex].Time;
			double DeltaTime = t2 - t1;
			double Factor = (ts - t1) / DeltaTime;

			auto t1Vec = BoneKey->ScaleKeyFrame[index].ScaleKey;
			auto t2Vec = BoneKey->ScaleKeyFrame[nextIndex].ScaleKey;

			return glm::mix(t1Vec, t2Vec, (float)Factor);
		}
		return glm::vec3();
	}
	glm::quat AnimationBone::CalcInterpolatedRotation(double ts, Ref<ModelKeyframeAnimation>& BoneKey)
	{
		if (type == Interpolation::Linear) {
			uint32_t index = FindRotationIndex(ts, BoneKey);
			uint32_t nextIndex = index + 1;
			if (nextIndex >= BoneKey->RotationKeyFrame.size()) 
			{
				glm::quat ro = BoneKey->RotationKeyFrame[index].Rotation;
				return ro;
			}

			double t1 = BoneKey->RotationKeyFrame[index].Time;
			double t2 = BoneKey->RotationKeyFrame[nextIndex].Time;
			double DeltaTime = t2 - t1;
			double Factor = (ts - t1) / DeltaTime;

			auto t1Mat = BoneKey->RotationKeyFrame[index].Rotation;
			auto t2Mat = BoneKey->RotationKeyFrame[nextIndex].Rotation;

			glm::quat ro = glm::mix(t1Mat, t2Mat, (float)Factor);
			return ro;
		}
		return glm::quat();
	}
	glm::vec3 AnimationBone::CalcInterpolatedPosition(double ts, Ref<ModelKeyframeAnimation>& BoneKey)
	{
		if (type == Interpolation::Linear) {
			uint32_t index = FindPositionIndex(ts, BoneKey);
			uint32_t nextIndex = index + 1;
			if (nextIndex >= BoneKey->PositionKeyFrame.size()) return BoneKey->PositionKeyFrame[index].Position;

			double t1 = BoneKey->PositionKeyFrame[index].Time;
			double t2 = BoneKey->PositionKeyFrame[nextIndex].Time;
			double DeltaTime = t2 - t1;
			double Factor = (ts - t1) / DeltaTime;

			auto t1Mat = BoneKey->PositionKeyFrame[index].Position;
			auto t2Mat = BoneKey->PositionKeyFrame[nextIndex].Position;

			return glm::mix(t1Mat, t2Mat, (float)Factor);
		}
		return glm::vec3();
	}
	uint32_t AnimationBone::FindPositionIndex(double ts, Ref<ModelKeyframeAnimation>& KeyFrame)
	{
		uint32_t size = KeyFrame->PositionKeyFrame.size();
		for (uint32_t i = 0; i < size - 1; i++) {
			double t = KeyFrame->PositionKeyFrame[i + 1].Time;
			if (ts < t) {
				return i;
			}
		}
		return 0;
	}
	uint32_t AnimationBone::FindRotationIndex(double ts, Ref<ModelKeyframeAnimation>& KeyFrame)
	{
		uint32_t size = KeyFrame->RotationKeyFrame.size();
		for (uint32_t i = 0; i < size - 1; i++) {
			double t = KeyFrame->RotationKeyFrame[i + 1].Time;
			if (ts < t) {
				return i;
			}
		}
		return 0;
	}
	uint32_t AnimationBone::FindScaleIndex(double ts, Ref<ModelKeyframeAnimation>& KeyFrame)
	{
		uint32_t size = KeyFrame->ScaleKeyFrame.size();
		for (uint32_t i = 0; i < size - 1; i++) {
			double t = KeyFrame->ScaleKeyFrame[i + 1].Time;
			if (ts < t) {
				return i;
			}
		}
		return 0;
	}
	void AnimationBone::Update(Kiana::Timestep ts)
	{
		current = preTime + (double)ts.GetMilliseconds() * 0.1;
		preTime = current;

		if (current > AnimationTime && loop)
		{
			current = AnimationTime;
			preTime -= AnimationTime;
		}
	}
	void AnimationBone::SetBoneMapIndex(std::unordered_map<std::string, uint32_t>& Bonde_name_to_index_map)
	{
		this->Bonde_name_to_index_map = Bonde_name_to_index_map;
	}
	void AnimationBone::Play()
	{

	}
	void AnimationBone::Play(Kiana::Timestep ts)
	{
		start = ts.GetMilliseconds();
		preTime = start;
	}
}