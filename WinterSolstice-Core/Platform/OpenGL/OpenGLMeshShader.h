#pragma once
#include "WinterSolstice/Render/Shader.h"
#include <glm/glm.hpp>

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace WinterSolstice {
	namespace Bronya {
		class OpenGLMeshShader :public Shader
		{
		public:
			OpenGLMeshShader(const std::string& name, const std::string& meshShaderSrc);
			virtual ~OpenGLMeshShader();

			virtual void Bind() const override;
			virtual void Unbind() const override;

			virtual void SetInt(const std::string& name, int value) override;
			virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
			virtual void SetUIntArray(const std::string& name, uint32_t* values, uint32_t count) override;
			virtual void SetFloat(const std::string& name, float value) override;
			virtual void SetFloatArray(const std::string& name, float* values, uint32_t count) override;
			virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
			virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
			virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
			virtual void SetMat4(const std::string& name, const glm::mat4& value) override;
			virtual void SetMat4s(const std::string& name, std::map<uint32_t, glm::mat4>& mat4s) override;

			virtual const std::string& GetName() const override { return m_Name; }
			virtual void SetName(std::string name) override;

			void UploadUniformInt(const std::string& name, int value);
			void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
			void UploadUniformUIntArray(const std::string& name, uint32_t* values, uint32_t count);

			void UploadUniformFloat(const std::string& name, float value);
			void UploadUniformFloatArray(const std::string& name, float* values, uint32_t count);
			void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
			void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
			void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

			void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
			void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		private:
			std::string ReadFile(const std::string& filepath);
			std::unordered_map<std::string, int> m_UniformLocationCache;
			int GetUniformLocation(const std::string& name, bool block = false);
		private:
			uint32_t m_RendererID;
			std::string m_FilePath;
			std::string m_Name;

			std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV;
			std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRV;

			std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;




			// 通过 Shader 继承
			void GenComputeResultBuffer(uint32_t size) override;

			void StartCompute(uint32_t x, uint32_t y, uint32_t z) override;

			void GetComputeResult(void* t) override;

		};
	}
}