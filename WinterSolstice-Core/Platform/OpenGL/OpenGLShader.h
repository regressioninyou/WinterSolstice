#pragma once

#include "WinterSolstice/Render/Shader.h"
#include <glm/glm.hpp>
#include <map>

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace WinterSolstice {
	namespace Bronya {

		class OpenGLShader : public Shader
		{
		public:
			OpenGLShader();
			OpenGLShader(const std::string& filepath);
			OpenGLShader(const std::string& vs, const std::string& fs);
			OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
			virtual ~OpenGLShader();

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

			virtual void GenComputeResultBuffer(uint32_t size) override {};
			virtual void StartCompute(uint32_t x = 1, uint32_t y = 1, uint32_t z = 1)override {};
			virtual void GetComputeResult(void* t) override {};

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
			void UploadUniformMat4s(const std::string& name, std::map<uint32_t, glm::mat4>& mat4s);
		private:

			std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);

			void CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
			void CompileOrGetOpenGLBinaries();
			void CreateProgram();

			void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);

			std::unordered_map<std::string, int> m_UniformLocationCache;
			int GetUniformLocation(const std::string& name, bool block = false);
		protected:
			std::string ReadFile(const std::string& filepath);
			void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
			uint32_t m_RendererID;
		private:
			std::string m_FilePath;
			std::string m_Name;

			std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV;
			std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRV;

			std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;
		};
		class OpenGLComputeShader:public OpenGLShader
		{
		public:
			OpenGLComputeShader(const std::string& filepath);
			virtual void GenComputeResultBuffer(uint32_t size) override;
			virtual void StartCompute(uint32_t x = 1, uint32_t y = 1, uint32_t z = 1)override;
			virtual void GetComputeResult(void* t) override;
		private:
			uint32_t resultBuffer;
			uint32_t resultBufferSize;
		};
	}
}
