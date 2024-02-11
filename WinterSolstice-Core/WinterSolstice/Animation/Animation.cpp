#include "k423.h"
#include "Animation.h"
#include "WinterSolstice/Animation/AnimationBone.h"
namespace WinterSolstice
{
	Ref<Animation> Animation::Create()
	{
		return CreateRef<AnimationBone>();
	}
	Ref<Animation> Animation::Create(const std::string& name)
	{
		return CreateRef<AnimationBone>(name);
	}
}