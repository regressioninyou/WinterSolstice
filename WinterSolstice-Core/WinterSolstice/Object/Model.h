#pragma once 
#include "Object.h"
namespace WinterSolstice {
	class BVH;
	class Model : public Object
	{
	public:
		ObjectClass GetObjectClass() override { return ObjectClass::Model; }
		void BuildBVH() override;

		// 通过 Object 继承
		void DrawBoundsLine() override;


		// 通过 Object 继承
		void BVHDrawBounds() override;
		void SetTranslation(Translation t) override;
		bool CheckBVHDown() override;
	private:
		bool buildDown = false;
		Ref<BVH> bvh = nullptr;

	};
}