#pragma once 
#include "WinterSolstice/Core/Timestep.h"
#include "k423.h"
struct aiBone;
namespace WinterSolstice {
	struct ModelKeyframeAnimation;
	class Animation
	{
	public:
		enum class Interpolation
		{
			Linear = 0, Bessel = 1
		};
	public:
		virtual ~Animation() = default;
		virtual void SetInterpolationFunc(Interpolation type) = 0;

		virtual void SetModelKeyFrame(const std::string& BoneName, Ref<ModelKeyframeAnimation> anim) = 0;
		virtual void AddModelKeyFrame(Ref<ModelKeyframeAnimation> anim) = 0;
		virtual void SetBoneMapIndex(std::unordered_map<std::string, uint32_t>& Bonde_name_to_index_map) = 0;
		virtual uint32_t GetBoneIndex(const std::string& BoneName) = 0;

		virtual glm::mat4 GetKeyFrame(const std::string& BoneName) = 0;
		virtual void SetLoop(bool loop) = 0;

		virtual uint32_t getBoueId(const aiBone* bone) = 0;
		virtual std::string GetName() = 0;
		virtual void SetName(const std::string& name) = 0;

		virtual void SetAnimationLoopTime(double time) = 0;

		virtual void Play() = 0;
		virtual void Play(Kiana::Timestep ts) = 0;

		virtual void Update(Kiana::Timestep ts) = 0;

		static Ref<Animation> Create();
		static Ref<Animation> Create(const std::string& name);
	};
}