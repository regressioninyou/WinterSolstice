#pragma once 
#include "WinterSolstice/Object/Object.h"
namespace WinterSolstice {

	class Triangle :public Object
	{
	public:
		Triangle() = default;
		Triangle(Point pos[3]) : point{ pos[0], pos[1], pos[2] }
		{
			setBounds(point[0].Position, point[1].Position, point[2].Position);
		}
		Point point[3];
		ObjectClass GetObjectClass() override { return ObjectClass::Triangle; }
		float IntersectP(const Ray& ray) override;

		// 通过 Object 继承
		void BuildBVH() override {};
		void DrawBoundsLine() override;

		void SetTranslation(Translation t) override;

		bool CheckBVHDown() override;
		void BVHDrawBounds() override;
	private:

	};
}