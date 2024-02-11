#pragma once 
#include "WinterSolstice/Animation/Animation.h"
namespace WinterSolstice {
	class AnimationBone :public Animation
	{
	public:
		AnimationBone() = default;
		AnimationBone(const std::string& n);
		virtual ~AnimationBone();

		virtual void SetModelKeyFrame(const std::string& BoneName, Ref<ModelKeyframeAnimation> anim) override;
		virtual void AddModelKeyFrame(Ref<ModelKeyframeAnimation> anim) override;
		void SetBoneMapIndex(std::unordered_map<std::string, uint32_t>& Bonde_name_to_index_map) override;

		virtual uint32_t GetBoneIndex(const std::string& BoneName) override;
		virtual glm::mat4 GetKeyFrame(const std::string& BoneName) override;
		virtual void SetLoop(bool loop) override;
		virtual uint32_t getBoueId(const aiBone* bone) override;
		virtual std::string GetName() override;
		virtual void SetName(const std::string& name) override;

		//设置动画插值方式
		virtual void SetInterpolationFunc(Interpolation type) override;

		virtual void SetAnimationLoopTime(double time) override;
		virtual void Play() override;
		virtual void Play(Kiana::Timestep ts) override;

		virtual void Update(Kiana::Timestep ts) override;
	private:
		glm::vec3 CalcInterpolatedScaling(double ts, Ref<ModelKeyframeAnimation>& BoneKey);
		glm::quat CalcInterpolatedRotation(double ts, Ref<ModelKeyframeAnimation>& BoneKey);
		glm::vec3 CalcInterpolatedPosition(double ts, Ref<ModelKeyframeAnimation>& BoneKey);

		uint32_t FindPositionIndex(double ts, Ref<ModelKeyframeAnimation>& KeyFrame);
		uint32_t FindRotationIndex(double ts, Ref<ModelKeyframeAnimation>& KeyFrame);
		uint32_t FindScaleIndex(double ts, Ref<ModelKeyframeAnimation>& KeyFrame);
	private:
		std::string name;
		std::unordered_map<std::string, uint32_t> Bonde_name_to_index_map;
		std::unordered_map<uint32_t, Ref<ModelKeyframeAnimation>> iKeyFrame;
		double start = 0.0f;
		double AnimationTime = 0.0f;
		double end = 0.0f;
		double preTime = 0.0f;
		double current = 0.0;
		bool loop = true;
		Interpolation type = Interpolation::Linear;

	};
}