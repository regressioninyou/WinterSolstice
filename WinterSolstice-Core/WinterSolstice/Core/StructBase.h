#pragma once 
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace WinterSolstice {
#define MAX_BONE_INFLUENCE 4
	inline static float Distance(const glm::vec3 target, const glm::vec3 point)
	{
		return ((target.x - point.x) * (target.x - point.x) +
			(target.y - point.y) * (target.y - point.y) +
			(target.z - point.z) * (target.z - point.z));
	}
	inline static glm::vec3 Min(const glm::vec3& p1, const glm::vec3& p2) {
		return glm::vec3(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
			std::min(p1.z, p2.z));
	}
	inline static glm::vec3 Max(const glm::vec3& p1, const glm::vec3& p2) {
		return glm::vec3(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
			std::max(p1.z, p2.z));
	}
	struct Point
	{
		// position
		glm::vec3 Position;
		// normal
		glm::vec3 Normal;
		// texCoords
		glm::vec2 TexCoords;
		// tangent
		glm::vec3 Tangent;
		// bitangent
		glm::vec3 Bitangent;
		//bone indexes which will influence this vertex
		int m_BoneIDs[MAX_BONE_INFLUENCE];
		//weights from each bone
		float m_Weights[MAX_BONE_INFLUENCE];
	};
	struct PositionKeyFrame
	{
		double Time;
		glm::vec3 Position;
	};
	struct RotationKeyFrame
	{
		double Time;
		glm::quat Rotation;
	};
	struct ScaleKeyFrame
	{
		double Time;
		glm::vec3 ScaleKey;
	};
	struct ModelKeyframeAnimation
	{
		std::string name;
		uint32_t index;
		std::vector<PositionKeyFrame> PositionKeyFrame;
		std::vector<RotationKeyFrame> RotationKeyFrame;
		std::vector<ScaleKeyFrame> ScaleKeyFrame;
	};
	struct Translation {
		glm::vec3 Transform = glm::vec3(0.0f);
		glm::vec3 Rotate = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);
		bool equals(Translation& orther) {
			if (Transform != orther.Transform) {
				return false;
			}
			if (Rotate != orther.Rotate) {
				return false;
			}
			if (Scale != orther.Scale) {
				return false;
			}
			return true;
		}
	};
	namespace Himeko {
		template<typename T>
		struct WokerInfo
		{
			std::function<T> WokerFunction;
			const char* FunctionInfo = "BaseRuntime Function";
			int ExecuteCount = 1;
		};
	}
}