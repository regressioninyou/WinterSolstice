#include "k423.h"
#include "Model.h"
#include "BVHImp.h"
namespace WinterSolstice {
	void Model::BuildBVH() {
		bvh = BVH::Create();
		bvh->buildBVH(objs);
		buildDown = true;
	}
	void Model::DrawBoundsLine()
	{
	}
	void Model::BVHDrawBounds()
	{
		if (bvh && buildDown)
			bvh->DrawBounds();
	}
	void Model::SetTranslation(Translation t)
	{
		Object::SetTranslation(t);
		bvh->FlushBVH(objs);
	}
	bool Model::CheckBVHDown()
	{
		return buildDown;
	}
}