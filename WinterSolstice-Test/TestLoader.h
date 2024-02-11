#pragma once 
#include <KyBao.h>
namespace KyBao {

	class TestModel
	{
	public:
		// model data 
		std::vector<Ref<Mesh>>    meshes;
		std::string directory;
		bool gammaCorrection;

		// constructor, expects a filepath to a 3D model.
		TestModel(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
		{
			//loadModel(path);
		}
		TestModel() {};
		// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes std::vector.
		void loadModel(std::string const& path)
		{
			// read file via ASSIMP
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
			// check for errors
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
			{
				//cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
				return;
			}
			// retrieve the directory path of the filepath
			directory = path.substr(0, path.find_last_of('/'));

			// process ASSIMP's root node recursively
			processNode(scene->mRootNode, scene);
		}

	private:
		std::unordered_map<std::string, Ref<Texture2D>> textures_loaded;

		// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void processNode(aiNode* node, const aiScene* scene)
		{
			// process each mesh located at the current node
			for (uint32_t i = 0; i < node->mNumMeshes; i++)
			{
				// the node object only contains indices to index the actual objects in the scene. 
				// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				//if (strcmp(mesh->mName.C_Str(), "hands")) {
				meshes.push_back(processMesh(mesh, scene));
				//}
			}
			// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for (uint32_t i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}

		}

		Ref<Mesh> processMesh(aiMesh* mesh, const aiScene* scene)
		{
			// data to fill
			std::vector<Point> vertices;
			std::vector<uint32_t> indices;
			std::vector<Ref<Texture2D>> textures;

			// walk through each of the mesh's vertices
			for (uint32_t i = 0; i < mesh->mNumVertices; i++)
			{
				Point vertex;
				glm::vec3 vector; // we declare a placeholder std::vector since assimp uses its own std::vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
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

				vertices.push_back(vertex);
			}
			// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for (uint32_t i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				// retrieve all indices of the face and store them in the indices std::vector
				for (uint32_t j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			// process materials
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
			// Same applies to other texture as the following list summarizes:
		 // 1. diffuse maps
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


			// return a mesh object created from the extracted mesh data
			Ref<VertexArray> va = VertexArray::Create();
			va->Bind();
			KyBao::Ref<KyBao::VertexBuffer> vb =
				KyBao::VertexBuffer::Create(vertices, sizeof(Point) * vertices.size());
			{
				KyBao::BufferLayout layout = {
					{KyBao::ShaderDataType::Float3,"aPosition"},
					{KyBao::ShaderDataType::Float3,"aNormal"},
					{KyBao::ShaderDataType::Float2,"aTexcoord"},
					{KyBao::ShaderDataType::Float3,"aTangent"},
					{KyBao::ShaderDataType::Float3,"aBitangent"},
					{KyBao::ShaderDataType::Int4,"aBoneIDs"},
					{KyBao::ShaderDataType::Float4,"aWeights"}
				};
				vb->SetLayout(layout);
			}

			va->Bind();
			KyBao::Ref<KyBao::IndexBuffer> ib =
				KyBao::IndexBuffer::Create(&indices[0], indices.size());
			va->AddVertexBuffer(vb);
			va->SetIndexBuffer(ib);
			Ref<Mesh> m = CreateRef<Mesh>();
			m->setVertexArray(va);
			for (auto& te : textures) {
				m->addTexture(te);
			}
			return m;
		}
		std::vector<Ref<Texture2D>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
		{
			std::vector<Ref<Texture2D>> texture;
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
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
				{   // 如果纹理还没有被加载，则加载它
					std::string source(str.C_Str());
					texture.emplace_back(Texture2D::Create(directory + "/" + source));
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
			return texture;
		}
	};
}