#pragma once 
#include "WinterSolstice/Object/Object.h"
#include "WinterSolstice/Object/DrawCall.h"
namespace WinterSolstice {
	class BVH;
	class Mesh :public Object , public DrawCall
	{
	public:
		Mesh();
		void setTriangles(std::vector<Ref<Object>> triangles) {
			this->objs = triangles;
		}
		void BuildBVH()override;;

		// 通过 Object 继承
		void DrawBoundsLine() override;
		bool CheckBVHDown() override;
		void BVHDrawBounds() override;

		ObjectClass GetObjectClass() override { return ObjectClass::Mesh; }
		void SetTranslation(Translation t) override;

		static Ref<Mesh> Cube();
		static Ref<Mesh> Plance();
	private:

		bool buildDown = false;
		Ref<BVH> bvh = nullptr;
	};
}