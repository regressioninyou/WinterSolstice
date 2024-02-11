#include "k423.h"
#include "Triangle.h"
#include "BVHImp.h"
namespace WinterSolstice {

	float Triangle::IntersectP(const Ray& ray) {
		const float EPSILON = 0.000001f;
		glm::vec3 edge1, edge2, h, s, q;
		float a, f, u, v, t;

		edge1 = point[1].Position - point[0].Position;
		edge2 = point[2].Position - point[0].Position;
		h = glm::cross(ray.direction, edge2);
		a = glm::dot(edge1, h);

		if (a > -EPSILON && a < EPSILON) {
			return -1.0f; // 射线平行于三角形
		}

		f = 1.0f / a;
		s = ray.origin - point[0].Position;
		u = f * glm::dot(s, h);

		if (u < 0.0f || u > 1.0f) {
			return -1.0f; // 射线超出三角形边界
		}

		q = glm::cross(s, edge1);
		v = f * glm::dot(ray.direction, q);

		if (v < 0.0f || u + v > 1.0f) {
			return -1.0f; // 射线超出三角形边界
		}

		t = f * glm::dot(edge2, q);

		if (t > EPSILON) {
			return t; // 射线与三角形相交
		}
		else {
			return -1.0f; // 射线在三角形后面或者与三角形平行
		}
	}
	void Triangle::DrawBoundsLine()
	{
	}
	void Triangle::SetTranslation(Translation t)
	{
		trans = t;
	}
	bool Triangle::CheckBVHDown()
	{
		return false;
	}
	void Triangle::BVHDrawBounds()
	{
	}
}