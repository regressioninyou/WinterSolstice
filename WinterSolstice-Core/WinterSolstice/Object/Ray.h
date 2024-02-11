#pragma once 
#include <glm/glm.hpp>
namespace WinterSolstice {
	struct  Ray
	{
	public:
		glm::vec3 origin;
		glm::vec3 direction, direction_inv;

		double t;
		double t_min, t_max;
		Ray(const glm::vec3& ori, const glm::vec3& dir, const double _t = 0.0) : origin(ori), direction(dir), t(_t) {
			direction_inv = glm::vec3(1. / direction.x, 1. / direction.y, 1. / direction.z);
			t_min = 0.0;
			t_max = std::numeric_limits<double>::max();
		}

		glm::vec3 operator()(double t) const { return origin + direction * (float)t; }
	};
}