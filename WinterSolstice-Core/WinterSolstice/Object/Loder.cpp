#include "k423.h"
#include "Loder.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "WinterSolstice/Render/Texture.h"
#include "WinterSolstice/Utils/ThreadPool.h"
#include <stack>
#include <WinterSolstice/Render/VertexArray.h>
#include "DrawCall.h"
#include <glm/gtc/type_ptr.hpp>
#include <WinterSolstice/Animation/Animation.h>
#include "WinterSolstice/Render/RendererCommand.h"
#include <WinterSolstice/Core/Application.h>
#include <Platform/load/stb_image/stb_image.h>
#include "WinterSolstice/Render/Shader.h"

namespace WinterSolstice {
	static glm::mat4 AssimpToGlm(const aiMatrix4x4& from) {
		glm::mat4 to;
		// 注意 Assimp 的矩阵是列主的，而 glm 默认是列主的，所以直接复制即可
		std::memcpy(glm::value_ptr(to), &from, sizeof(aiMatrix4x4));
		return to;
	}
	static glm::vec3 AssimpCastGlmV3(const aiVector3D& from)
	{
		return glm::vec3(from.x, from.y, from.z);
	}
	static glm::quat AssimpCastGlmV4(const aiQuaternion& from)
	{
		return glm::quat(from.x, from.y, from.z, from.z);
	}
	static glm::mat4 AssimpCastGlmM4(const aiMatrix4x4& assimpMatrix) {
		glm::mat4 glmMatrix{ 1.0f };

		// 逐个元素复制
		glmMatrix[0][0] = assimpMatrix.a1; glmMatrix[1][0] = assimpMatrix.a2;
		glmMatrix[2][0] = assimpMatrix.a3; glmMatrix[3][0] = assimpMatrix.a4;

		glmMatrix[0][1] = assimpMatrix.b1; glmMatrix[1][1] = assimpMatrix.b2;
		glmMatrix[2][1] = assimpMatrix.b3; glmMatrix[3][1] = assimpMatrix.b4;

		glmMatrix[0][2] = assimpMatrix.c1; glmMatrix[1][2] = assimpMatrix.c2;
		glmMatrix[2][2] = assimpMatrix.c3; glmMatrix[3][2] = assimpMatrix.c4;

		glmMatrix[0][3] = assimpMatrix.d1; glmMatrix[1][3] = assimpMatrix.d2;
		glmMatrix[2][3] = assimpMatrix.d3; glmMatrix[3][3] = assimpMatrix.d4;

		return glmMatrix;
	}



	Loder::Loder()
	{
		Computed = Bronya::Shader::CreateCompute("./assets/shaders/compute_shader.glsl");
	}

	Ref<Object> Loder::loadOBJ(const std::string& filepath) {
		Assimp::Importer importer;
		//const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | 
		//	aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights | 
		//	aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | 
		//	aiProcess_OptimizeGraph | aiProcess_GenSmoothNormals | 
		//	aiProcess_FixInfacingNormals | aiProcess_PreTransformVertices | 
		//	aiProcess_ValidateDataStructure);
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return nullptr;
		}

		std::string path(filepath.c_str());
		auto lasSlash = path.find_last_of("/\\");
		CachePath = filepath.substr(0, lasSlash);

		Ref<Object> model = processNode(scene->mRootNode, scene);
		bone = CreateRef<Bone>();
		bone->indexBoneMap = Bonde_name_to_index_map;
		if (scene->mNumAnimations > 0) {
			for (uint32_t i = 0; i < scene->mNumAnimations; i++) {
				auto* Animate = scene->mAnimations[i];
				animation = Animation::Create();
				animation->SetBoneMapIndex(Bonde_name_to_index_map);
				animation->SetName(Animate->mName.C_Str());
				animation->SetAnimationLoopTime(Animate->mDuration);
				for (uint32_t a = 0; a < Animate->mNumChannels; a++)
				{
					const auto* nodeAnim = Animate->mChannels[a];
					Ref<ModelKeyframeAnimation> nodeKeyFrameAnimation = CreateRef<ModelKeyframeAnimation>();
					nodeKeyFrameAnimation->name = nodeAnim->mNodeName.C_Str();
					for (uint32_t j = 0; j < nodeAnim->mNumPositionKeys; j++) {
						const auto posNodeAnim = nodeAnim->mPositionKeys[j];
						nodeKeyFrameAnimation->PositionKeyFrame.push_back({ posNodeAnim.mTime,AssimpCastGlmV3(posNodeAnim.mValue) });
					}
					for (uint32_t j = 0; j < nodeAnim->mNumRotationKeys; j++) {
						const auto rotateNodeAnim = nodeAnim->mRotationKeys[j];
						nodeKeyFrameAnimation->RotationKeyFrame.push_back({ rotateNodeAnim.mTime,AssimpCastGlmV4(rotateNodeAnim.mValue) });
					}
					for (uint32_t j = 0; j < nodeAnim->mNumScalingKeys; j++) {
						const auto scalNodeAnim = nodeAnim->mScalingKeys[j];
						nodeKeyFrameAnimation->ScaleKeyFrame.push_back({ scalNodeAnim.mTime,AssimpCastGlmV3(scalNodeAnim.mValue) });
					}
					//KyBao_CORE_INFO("Animation Key Name:{0}", nodeAnim->mNodeName.C_Str());
					animation->SetModelKeyFrame(nodeAnim->mNodeName.C_Str(), nodeKeyFrameAnimation);
				}
				bone->AddAnimation(animation);
			}
		}
#if 0
		auto* Animate = scene->mAnimations[0];
		animation->SetName(Animate->mName.C_Str());
		for (uint32_t i = 0; i < Animate->mNumChannels; i++)
		{
			const auto* nodeAnim = Animate->mChannels[i];
			//if (Bonde_name_to_index_map.find(std::string(nodeAnim->mNodeName.C_Str())) == Bonde_name_to_index_map.end())
			//{
			//	KyBao_CORE_INFO("Not Find Name {0} in Bonde_name_to_index_map", nodeAnim->mNodeName.C_Str());
			//}
			//else {
			//	KyBao_CORE_INFO("Had Name {0} in Bonde_name_to_index_map in {1}", nodeAnim->mNodeName.C_Str(), Bonde_name_to_index_map[std::string(nodeAnim->mNodeName.C_Str())]);
			//}
			Ref<ModelKeyframeAnimation> nodeKeyFrameAnimation = CreateRef<ModelKeyframeAnimation>();
			nodeKeyFrameAnimation->name = nodeAnim->mNodeName.C_Str();
			for (uint32_t j = 0; j < nodeAnim->mNumPositionKeys; j++) {
				const auto posNodeAnim = nodeAnim->mPositionKeys[j];
				nodeKeyFrameAnimation->PositionKeyFrame.push_back({ posNodeAnim.mTime,AssimpCastGlmV3(posNodeAnim.mValue) });
				int x = 0;
			}
			for (uint32_t j = 0; j < nodeAnim->mNumRotationKeys; j++) {
				const auto rotateNodeAnim = nodeAnim->mRotationKeys[j];
				nodeKeyFrameAnimation->RotationKeyFrame.push_back({ rotateNodeAnim.mTime,AssimpCastGlmV4(rotateNodeAnim.mValue) });
				int x = 0;
			}
			for (uint32_t j = 0; j < nodeAnim->mNumScalingKeys; j++) {
				const auto scalNodeAnim = nodeAnim->mScalingKeys[j];
				nodeKeyFrameAnimation->ScaleKeyFrame.push_back({ scalNodeAnim.mTime,AssimpCastGlmV3(scalNodeAnim.mValue) });
				int x = 0;
			}
			KyBao_CORE_INFO("Animation Key Name:{0}", nodeAnim->mNodeName.C_Str());
			animation->SetModelKeyFrame(nodeAnim->mNodeName.C_Str(), nodeKeyFrameAnimation);
		}
#endif
		root = CreateRef<BoneNode>();
		root->name = "Root";
		root->index = -1;
		root->OffsetMatrix = glm::mat4(1.0f);
		ReadNodeHierarchy(scene->mRootNode, scene, root);
		bone->SetRoot(root);
		glm::mat4 mat = AssimpCastGlmM4(scene->mRootNode->mTransformation);
		bone->Bone_Offset_ZERO = glm::inverse(mat);
		model->SetName(scene->mRootNode->mName.C_Str());
		ObjectRmoveBone(model);
		return model;
	}
	void Loder::precessMaterial(const aiScene* scene)
	{
#if 0
		std::vector<Bronya::TextureSpecification> textureSpecifications;
		std::vector<stbi_uc*> textureSpecifications;
		for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
			auto* material = scene->mMaterials[i];
			for (unsigned int i = 0; i < material->GetTextureCount; i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				for (const auto& tex : textures_loaded)
				{
					if (std::strcmp(tex.first.c_str(), str.C_Str()) == 0) {
						texture.emplace_back(tex.second);
						return texture;
					}
				}

				Bronya::TextureSpecification texSpec;
				stbi_uc* data = nullptr;
				{
					data = stbi_load(, &width, &height, &channels, 0);
				}
				if (data) {
					std::string source(str.C_Str());
					texture.emplace_back(Bronya::RenderCommand::GetTexture2D(CachePath + "\\" + source));
					std::string path(str.C_Str());
					auto lasSlash = path.find_last_of("/\\");
					lasSlash = lasSlash == std::string::npos ? 0 : lasSlash + 1;
					auto lastDot = path.rfind('.');
					auto count = lastDot == std::string::npos ? path.size() - lasSlash : lastDot - lasSlash;
					std::string name = path.substr(lasSlash, count);
				}
				{   // 如果纹理还没有被加载，则加载它
					std::string source(str.C_Str());
					texture.emplace_back(Bronya::RenderCommand::GetTexture2D(CachePath + "\\" + source));
					std::string path(str.C_Str());
					auto lasSlash = path.find_last_of("/\\");
					lasSlash = lasSlash == std::string::npos ? 0 : lasSlash + 1;
					auto lastDot = path.rfind('.');
					auto count = lastDot == std::string::npos ? path.size() - lasSlash : lastDot - lasSlash;
					std::string name = path.substr(lasSlash, count);

					texture[i]->setPath(path);
					texture[i]->setName(name);
					texture[i]->setType(typeName);
					textures_loaded[str.C_Str()] = texture[i]; // 添加到已加载的纹理中
					return texture;
				}
			}
}
#endif
	}
	//static ThreadPool thp(6);
	Ref<Object> Loder::processNode(aiNode* node, const aiScene* scene)
	{
		Ref<Object> model = Object::Create(ObjectClass::Model);
		model->SetName(node->mName.C_Str());
		// 处理节点所有的网格（如果有的话）
		glm::vec3 min_vert = glm::vec3{ std::numeric_limits<float>::infinity(),
									 std::numeric_limits<float>::infinity(),
									 std::numeric_limits<float>::infinity() };
		glm::vec3 max_vert = glm::vec3{ -std::numeric_limits<float>::infinity(),
									 -std::numeric_limits<float>::infinity(),
									 -std::numeric_limits<float>::infinity() };
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			auto child = processMesh(mesh, scene);
			Ref<Bounds3> bounds = child->GetBounds();
			min_vert = Min(min_vert, bounds->pMin);
			max_vert = Max(max_vert, bounds->pMax);
			model->AddObject(child);
		}
		// 接下来对它的子节点重复这一过程
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			auto child = processNode(node->mChildren[i], scene);

			if (child->GetOBJ().size() == 0)
			{
				continue;
			}

			Ref<Bounds3> bounds = child->GetBounds();
			min_vert = Min(min_vert, bounds->pMin);
			max_vert = Max(max_vert, bounds->pMax);
			model->AddObject(child);
		}
		if (model->GetOBJ().size() == 1)
			return model->GetOBJ()[0];
		model->setBounds(min_vert, max_vert);
		model->BuildBVH();
		return model;
	}
	uint32_t Loder::getBoueId(const aiBone* bone) {
		std::string name = std::string(bone->mName.C_Str());
		//uint32_t id = 0;
		if (Bonde_name_to_index_map.find(name) == Bonde_name_to_index_map.end())
		{
			Bonde_name_to_index_map[name] = Bonde_name_to_index_map.size();
		}
		/*else {
			id = Bonde_name_to_index_map[bone->mName.C_Str()];
		}*/
		return Bonde_name_to_index_map[name];
	}
	static aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& NodeName)
	{
		for (uint32_t i = 0; i < pAnimation->mNumChannels; i++) {
			aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

			if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
				return pNodeAnim;
			}
		}

		return NULL;
	}
	void Loder::ReadNodeHierarchy(const aiNode* pNode, const aiScene* pScene, Ref<BoneNode> node)
	{
		std::string NodeName(pNode->mName.C_Str());
		if (Bone_Offset_map.find(NodeName) == Bone_Offset_map.end())
		{
			for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			{
				ReadNodeHierarchy(pNode->mChildren[i], pScene, node);
			}
			return;
		}
		auto bnode = Bone_Offset_map[NodeName];
		bnode->name = NodeName;
		bnode->parent = node;
		bnode->BaseTransform = AssimpCastGlmM4(pNode->mTransformation);
		auto parent = pNode->mParent;
		std::string parentName(parent->mName.C_Str());
		//if (Bone_Offset_map.find(parentName) == Bone_Offset_map.end())
		{
			node->child.push_back(bnode);
			for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			{
				ReadNodeHierarchy(pNode->mChildren[i], pScene, bnode);
			}
			return;
		}
	}
	void Loder::ObjectRmoveBone(Ref<Object> obj)
	{
		for (auto it = obj->GetOBJ().begin(); it != obj->GetOBJ().end(); ++it) {
			if (Bone_Offset_map.find(it->get()->GetName()) != Bone_Offset_map.end()) {
				// 标记待删除的元素
				it->reset(); // 或者设置某个标记，表示需要删除
			}
			else {
				ObjectRmoveBone(*it);
			}
		}

		// 删除已标记的元素
		obj->GetOBJ().erase(
			std::remove_if(obj->GetOBJ().begin(), obj->GetOBJ().end(),
				[](const Ref<Object>& o) { return !o; }),
			obj->GetOBJ().end()
		);
	}
	static Point getPoint(aiMesh* mesh, uint32_t i) {
		Point vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		for (int i = 0; i < 4; ++i) {
			vertex.m_Weights[i] = 0.0f;
			vertex.m_BoneIDs[i] = 0.0f;
		}
		return vertex;
	}

	std::string Loder::SwitchTextureType(aiTextureType textureType) {
		std::string typeName = "Unknown";
		switch (textureType) {
		case aiTextureType_NONE:
			typeName = "None";
			break;
		case aiTextureType_DIFFUSE:
			typeName = "Diffuse";
			break;
		case aiTextureType_SPECULAR:
			typeName = "Specular";
			break;
		case aiTextureType_AMBIENT:
			typeName = "Ambient";
			break;
		case aiTextureType_EMISSIVE:
			typeName = "Emissive";
			break;
		case aiTextureType_HEIGHT:
			typeName = "Height";
			break;
		case aiTextureType_NORMALS:
			typeName = "Normals";
			break;
		case aiTextureType_SHININESS:
			typeName = "Shininess";
			break;
		case aiTextureType_OPACITY:
			typeName = "Opacity";
			break;
		case aiTextureType_DISPLACEMENT:
			typeName = "Displacement";
			break;
		case aiTextureType_LIGHTMAP:
			typeName = "Lightmap";
			break;
		case aiTextureType_REFLECTION:
			typeName = "Reflection";
			break;
		case aiTextureType_BASE_COLOR:
			typeName = "BaseColor";
			break;
		case aiTextureType_NORMAL_CAMERA:
			typeName = "NormalCamera";
			break;
		case aiTextureType_EMISSION_COLOR:
			typeName = "EmissionColor";
			break;
		case aiTextureType_METALNESS:
			typeName = "Metalness";
			break;
		case aiTextureType_DIFFUSE_ROUGHNESS:
			typeName = "DiffuseRoughness";
			break;
		case aiTextureType_AMBIENT_OCCLUSION:
			typeName = "AmbientOcclusion";
			break;
		case aiTextureType_SHEEN:
			typeName = "Sheen";
			break;
		case aiTextureType_CLEARCOAT:
			typeName = "Clearcoat";
			break;
		case aiTextureType_TRANSMISSION:
			typeName = "Transmission";
			break;
		case aiTextureType_UNKNOWN:
			typeName = "Unknown";
			break;
		case _aiTextureType_Force32Bit:
			typeName = "Force32Bit";
			break;
		default:
			typeName = "Unknown";
			break;
		}
		return typeName;
	}
	Ref<Object> Loder::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector <Ref<Object>> triangles;
		std::vector <Point> points;
		std::vector <uint32_t> index;
		points.reserve(mesh->mNumVertices + 1);
		index.reserve(mesh->mNumVertices * 3 + 1);

		Ref<Object> child = Object::Create(ObjectClass::Mesh);
		child->SetName(mesh->mName.C_Str());
		DrawCall* drawCall = dynamic_cast<DrawCall*>(child.get());

		glm::vec3 min_vert = glm::vec3{ std::numeric_limits<float>::infinity(),
									 std::numeric_limits<float>::infinity(),
									 std::numeric_limits<float>::infinity() };
		glm::vec3 max_vert = glm::vec3{ -std::numeric_limits<float>::infinity(),
									 -std::numeric_limits<float>::infinity(),
									 -std::numeric_limits<float>::infinity() };
		// 处理索引
		{
			auto task1 = Application::GetThreadPool().AnsycTask(
				[&]()->int
				{
					for (unsigned int i = 0; i < mesh->mNumVertices; i++)
					{
						points.push_back(getPoint(mesh, i));
					}
					//});
					if (mesh->HasBones()) {
						for (uint32_t i = 0; i < mesh->mNumBones; i++) {
							aiBone* bone = mesh->mBones[i];
							if (Bone_Offset_map.find(std::string(bone->mName.C_Str())) == Bone_Offset_map.end()) {
								auto bb = CreateRef<BoneNode>(AssimpCastGlmM4(bone->mOffsetMatrix), std::string(bone->mName.C_Str()), getBoueId(bone));
								Bone_Offset_map[std::string(bone->mName.C_Str())] = bb;
							}
							//mesh->mAnimMeshes
							//bone->mNode;
							for (uint32_t j = 0; j < bone->mNumWeights; j++) {
								const auto& vw = bone->mWeights[j];
								auto id = vw.mVertexId;
								for (short x = 0; x < MAX_BONE_INFLUENCE; x++) {
									if (points[id].m_Weights[x] == 0.0) {
										points[id].m_BoneIDs[x] = getBoueId(bone);
										points[id].m_Weights[x] = vw.mWeight;
										break;
									}
								}
							}
						}
					}
					return 1;
				}
			);
			auto task2 = Application::GetThreadPool().AnsycTask(
				[&]() ->int
				{
					for (unsigned int i = 0; i < mesh->mNumFaces; i++)
					{
						aiFace face = mesh->mFaces[i];
						//retrieve all indices of the face and store them in the indices vector
						if (face.mNumIndices == 3) {
							uint32_t at1 = face.mIndices[0];
							uint32_t at2 = face.mIndices[1];
							uint32_t at3 = face.mIndices[2];
							index.push_back(at1);
							index.push_back(at2);
							index.push_back(at3);
							Point point[3];
							point[0] = getPoint(mesh, at1);
							point[1] = getPoint(mesh, at2);
							point[2] = getPoint(mesh, at3);

							Ref<Object> triangle = Object::CreateTriangle(point);

							Ref<Bounds3> bound = triangle->GetBounds();
							min_vert = glm::vec3(std::min(min_vert.x, bound->pMin.x), std::min(min_vert.y, bound->pMin.y), std::min(min_vert.z, bound->pMin.z));
							max_vert = glm::vec3(std::max(max_vert.x, bound->pMax.x), std::max(max_vert.y, bound->pMax.y), std::max(max_vert.z, bound->pMax.z));
							triangles.emplace_back(triangle);
						}
						else
							for (unsigned int j = 0; j < face.mNumIndices; j++)
							{
								index.push_back(face.mIndices[j]);
							}
					}
					return 1;
				}
			);
			task1.get();
			task2.get();
		}
		Ref<Bronya::VertexArray> va = Bronya::RenderCommand::GetVertexArray();
		va->Bind();
		Ref<Bronya::VertexBuffer> vb =
			Bronya::RenderCommand::GetVertexBuffer(points, sizeof(Point) * points.size());
		{
			Bronya::BufferLayout layout = {
				{Bronya::ShaderDataType::Float3,"aPosition"},
				{Bronya::ShaderDataType::Float3,"aNormal"},
				{Bronya::ShaderDataType::Float2,"aTexcoord"},
				{Bronya::ShaderDataType::Float3,"aTangent"},
				{Bronya::ShaderDataType::Float3,"aBitangent"},
				{Bronya::ShaderDataType::Int4,"aBoneIDs"},
				{Bronya::ShaderDataType::Float4,"aWeights"}
			};
			vb->SetLayout(layout);
		}

		Ref<Bronya::IndexBuffer> ib =
			Bronya::RenderCommand::GetIndexBuffer(&index[0], index.size());
		va->AddVertexBuffer(vb);
		va->SetIndexBuffer(ib);
		// 处理材质
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			{
				std::vector<Ref<Bronya::Texture2D>> textures;
				std::vector<std::future<std::vector<Ref<Bronya::Texture2D>>>> ress;
#if 1
				for (aiTextureType c = aiTextureType_NONE; c <= aiTextureType_UNKNOWN; c = static_cast<aiTextureType>(c + 1)) {
					std::string type = SwitchTextureType(c);
					std::vector<Ref<Bronya::Texture2D>> t = loadMaterialTextures(material, c, type);
					textures.insert(textures.end(), t.begin(), t.end());
				}
#else
				std::vector<Ref<Texture2D>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
				// 2. specular maps
				std::vector<Ref<Texture2D>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
				// 3. normal maps
				std::vector<Ref<Texture2D>> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
				textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
				// 4. height maps
				std::vector<Ref<Texture2D>> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
				textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
#endif 
				if (textures.size())
				{
					for (auto& t : textures) {
						drawCall->addTexture(t);
					}
				}
				}
			}

		drawCall->setVertexArray(va);
		child->setBounds(min_vert, max_vert);

		child->AddObjects(triangles);
		child->BuildBVH();
		return child;
		}
	std::vector<Ref<Bronya::Texture2D>> Loder::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<Ref<Bronya::Texture2D>> texture;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			//for (const auto& tex : textures_loaded)
			//{
			if (textures_loaded.find(str.C_Str()) != textures_loaded.end()) {
				texture.emplace_back(textures_loaded[str.C_Str()]);
				//return textur;
				continue;
			}
			//if (std::strcmp(tex.first.c_str(), str.C_Str()) == 0) {
			//}
		//}
			{   // 如果纹理还没有被加载，则加载它
				std::string source(str.C_Str());
				texture.emplace_back(Bronya::RenderCommand::GetTexture2D(CachePath + "\\" + source));
				std::string path(str.C_Str());
				auto lasSlash = path.find_last_of("/\\");
				lasSlash = lasSlash == std::string::npos ? 0 : lasSlash + 1;
				auto lastDot = path.rfind('.');
				auto count = lastDot == std::string::npos ? path.size() - lasSlash : lastDot - lasSlash;
				std::string name = path.substr(lasSlash, count);
				texture[i]->setPath(path);
				texture[i]->setName(name);
				texture[i]->setType(typeName);
				texture[i]->SetUse(true);

				Computed->GenComputeResultBuffer(sizeof(int));
				Computed->Bind();
				texture[i]->Bind();
				Computed->SetInt("texWidth", texture[i]->GetWidth());
				Computed->SetInt("texHeight", texture[i]->GetHeight());
				Computed->StartCompute();
				int* result = new int;
				Computed->GetComputeResult(result);
				if (*result == 1) {
					texture[i]->SetTranslucent(true);
				}
				delete result;

				textures_loaded[str.C_Str()] = texture[i]; // 添加到已加载的纹理中
				return texture;
			}
		}
		return texture;
	}
	}