#include "k423.h"
#include "Bounds3.h"
#include <glm/gtx/quaternion.hpp>

namespace WinterSolstice {
	Bounds3::Bounds3()
	{
		double minNum = std::numeric_limits<double>::lowest();
		double maxNum = std::numeric_limits<double>::max();
		pMax = glm::vec3(minNum, minNum, minNum);
		pMin = glm::vec3(maxNum, maxNum, maxNum);
	}
	Bounds3::Bounds3(const glm::vec3 p) : pMin(p), pMax(p) {}
	Bounds3::Bounds3(const glm::vec3 p1, const glm::vec3 p2)
	{
		pMin = glm::vec3(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
		pMax = glm::vec3(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
	}
	Bounds3::Bounds3(const glm::vec3 p1, const glm::vec3 p2, const glm::vec3 p3)
	{
		pMin = glm::vec3(fmin(p1.x, fmin(p2.x, p3.x)), fmin(p1.y, fmin(p2.y, p3.y)), fmin(p1.z, fmin(p2.z, p3.z)));
		pMax = glm::vec3(fmax(p1.x, fmax(p2.x, p3.x)), fmax(p1.y, fmax(p2.y, p3.y)), fmax(p1.z, fmax(p2.z, p3.z)));
	}
	glm::vec3 Bounds3::Diagonal() const { return pMax - pMin; }
	int Bounds3::maxExtent() const
	{
		glm::vec3 d = Diagonal();
		if (d.x > d.y && d.x > d.z)
			return 0;
		else if (d.y > d.z)
			return 1;
		else
			return 2;
	}
	double Bounds3::SurfaceArea() const
	{
		glm::vec3 d = Diagonal();
		return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
	}
	glm::vec3 Bounds3::Centroid() { return 0.5f * pMin + 0.5f * pMax; }
	Bounds3 Bounds3::Intersect(const Bounds3& b)
	{
		return Bounds3(glm::vec3(fmax(pMin.x, b.pMin.x), fmax(pMin.y, b.pMin.y),
			fmax(pMin.z, b.pMin.z)),
			glm::vec3(fmin(pMax.x, b.pMax.x), fmin(pMax.y, b.pMax.y),
				fmin(pMax.z, b.pMax.z)));
	}
	glm::vec3 Bounds3::Offset(const glm::vec3& p) const
	{
		glm::vec3 o = p - pMin;
		if (pMax.x > pMin.x)
			o.x /= pMax.x - pMin.x;
		if (pMax.y > pMin.y)
			o.y /= pMax.y - pMin.y;
		if (pMax.z > pMin.z)
			o.z /= pMax.z - pMin.z;
		return o;
	}
	bool Bounds3::Overlaps(const Bounds3& b1, const Bounds3& b2)
	{
		bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
		bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
		bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
		return (x && y && z);
	}
	bool Bounds3::Inside(const glm::vec3& p, const Bounds3& b)
	{
		return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y &&
			p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
	}
	void Bounds3::UpDate(glm::mat4 trans)
	{
		glm::vec3 originalMinPoint = glm::vec3(std::numeric_limits<float>::infinity());
		glm::vec3 originalMaxPoint = glm::vec3(-std::numeric_limits<float>::infinity());
		for (int i = 0; i < 8; ++i) {
			glm::vec3 vertex = glm::vec3(
				(i & 1) ? pMax.x : pMin.x,
				(i & 2) ? pMax.y : pMin.y,
				(i & 4) ? pMax.z : pMin.z
			);

			glm::vec4 transformedVertex = trans * glm::vec4(vertex, 1.0f);
			glm::vec3 transformedPoint = glm::vec3(transformedVertex);

			originalMinPoint = glm::min(originalMinPoint, vertex);
			originalMaxPoint = glm::max(originalMaxPoint, vertex);
		}
		pMin = originalMinPoint;
		pMax = originalMaxPoint;
	}
	const glm::vec3& Bounds3::operator[](int i) const
	{
		return (i == 0) ? pMin : pMax;
	}
	bool Bounds3::IntersectP(const Ray& ray, const glm::vec3& invDir, const std::array<int, 3>& dirisNeg) const {
		glm::vec3 tmin = (pMin - ray.origin) * invDir;
		glm::vec3 tmax = (pMax - ray.origin) * invDir;

		if (!dirisNeg[0])
			std::swap(tmin.x, tmax.x);
		if (!dirisNeg[1])
			std::swap(tmin.y, tmax.y);
		if (!dirisNeg[2])
			std::swap(tmin.z, tmax.z);
		float tenter = std::max(tmax.x, std::max(tmax.y,tmax.z));

		float texit = std::min(tmin.x, std::min(tmin.y,tmin.z));
		return tenter <= texit && texit >= 0;
	}
	Ref<Bounds3> Union(const Ref<Bounds3> b1, const Ref<Bounds3> b2)
	{
		Ref<Bounds3> ret = CreateRef<Bounds3>();
		ret->pMin = Min(b1->pMin, b2->pMin);
		ret->pMax = Max(b1->pMax, b2->pMax);
		return ret;
	}
	Ref<Bounds3> Union(const Bounds3& b1, const Bounds3& b2)
	{
		Ref<Bounds3> ret = CreateRef<Bounds3>();
		ret->pMin = Min(b1.pMin, b2.pMin);
		ret->pMax = Max(b1.pMax, b2.pMax);
		return ret;
	}
	Ref<Bounds3> Union(const Bounds3& b, const glm::vec3& p)
	{
		Ref<Bounds3> ret = CreateRef<Bounds3>();
		ret->pMin = Min(b.pMin, p);
		ret->pMax = Max(b.pMax, p);
		return ret;
	}
}