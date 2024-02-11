#pragma once 
#include <glm/glm.hpp>
#include "WinterSolstice/Core/Core.h"
#include "Object.h"
#include "WinterSolstice/Render/Texture.h"
#include <mutex>
#include <WinterSolstice/Animation/Bone.h>
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiBone;
struct aiMaterial;
enum aiTextureType;
namespace WinterSolstice {
	class Animation;
	namespace Bronya {
		class Shader;
	}
	class Loder
	{
	public:
		Loder();
		Ref<Object> loadOBJ(const std::string& filepath);
		static Ref<Loder> Get() { return loder; }
		Ref<Animation> animation;
		Ref<Bone> bone;
	private:
		void precessMaterial(const aiScene* scene);
		Ref<Object> processNode(aiNode* node, const aiScene* scene);
		Ref<Object> processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Ref<Bronya::Texture2D>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
		static std::string SwitchTextureType(aiTextureType textureType);
		uint32_t getBoueId(const aiBone* bone);

		void ReadNodeHierarchy(const aiNode* pNode, const aiScene* scene, Ref<BoneNode> node);
		void ObjectRmoveBone(Ref<Object> obj);
	private:
		std::unordered_map<std::string, Ref<Bronya::Texture2D>> textures_loaded;

		std::unordered_map<std::string, uint32_t> Bonde_name_to_index_map;

		std::unordered_map<std::string, Ref<BoneNode>> Bone_Offset_map;

		Ref<BoneNode> root;
		Ref<Bronya::Shader> Computed;

		std::string CachePath;
		std::mutex mutexx;
		static Ref<Loder> loder;
	};
}