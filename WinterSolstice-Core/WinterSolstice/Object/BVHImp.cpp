#include "k423.h"
#include "BVHImp.h"
#include "BVH.h"
namespace WinterSolstice {
	Ref<BVH> BVH::Create()
	{
		return CreateRef<BVHAccel>();
	}
}