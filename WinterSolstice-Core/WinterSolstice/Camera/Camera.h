#pragma once 
#include <glm/glm.hpp>

namespace WinterSolstice {
	namespace Rossweisse {
		class Camera
		{
		private:
			/* data */
		public:
			Camera() = default;
			Camera(const glm::mat4& proj)
				:m_Projection(proj) {};

			virtual ~Camera() = default;
			const glm::mat4& GetProjection() const { return m_Projection; };
		protected:
			glm::mat4 m_Projection;
		};
	}
}