#pragma once 
#include <string>
#include <map>

namespace WinterSolstice {
	namespace Bronya {
		class Shader
		{
		public:
			virtual ~Shader() = default;
			virtual void GenComputeResultBuffer(uint32_t size) = 0;
			virtual void StartCompute(uint32_t x = 1, uint32_t y = 1, uint32_t z = 1) = 0; 
			virtual void GetComputeResult(void* t) = 0;

			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual void SetInt(const std::string& name, int value) = 0;
			virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
			virtual void SetUIntArray(const std::string& name, uint32_t* values, uint32_t count) = 0;
			virtual void SetFloat(const std::string& name, float value) = 0;
			virtual void SetFloatArray(const std::string& name, float* values, uint32_t count) = 0;
			virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
			virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
			virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
			virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
			virtual void SetMat4s(const std::string& name, std::map<uint32_t, glm::mat4>& mat4s) = 0;

			virtual const std::string& GetName() const = 0;
			virtual void SetName(std::string name) = 0;

			static Ref<Shader> Create(const std::string& filepath);
			static Ref<Shader> CreateCompute(const std::string& filepath);
			static Ref<Shader> Create(const std::string& vs, const std::string& fs);
			static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
			static Ref<Shader> CreateMeshShader(const std::string& name, const std::string& meshShaderSrc);
		};
		class ShaderLibrary
		{
		public:
			void Add(const std::string& name, const Ref<Shader>& shader);
			void Add(const Ref<Shader>& shader);
			Ref<Shader> Load(const std::string& filepath);
			Ref<Shader> Load(const std::string& name, const std::string& filepath);
			Ref<Shader> Load(const std::string& name, const std::string& vs, const std::string& fs);

			Ref<Shader> Get(const std::string& name);
			Ref<Shader> GetAs(const std::string& name);

			bool Exists(const std::string& name) const;

			inline auto begin() { return m_Shaders.begin(); }
			inline auto end() { return m_Shaders.end(); }
		private:
			std::unordered_map<std::string, Ref<Shader>> m_Shaders;
		};
	}
}