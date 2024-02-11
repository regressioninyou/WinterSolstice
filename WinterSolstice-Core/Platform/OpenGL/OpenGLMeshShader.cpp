#include "k423.h"
#include "OpenGLMeshShader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <WinterSolstice/Core/Time.h>

namespace WinterSolstice {
	namespace Bronya {

		void compileShader(GLuint shader, const char* source) {
			glShaderSource(shader, 1, &source, nullptr);
			glCompileShader(shader);

			GLint success;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

			if (!success) {
				GLchar infoLog[512];
				glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
				std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
			}
		}


		OpenGLMeshShader::OpenGLMeshShader(const std::string& name, const std::string& meshShaderSrc)
		{
#define GL_MESH_SHADER_NV 0x9559

			std::string meshShaderSource;
			if (meshShaderSrc.size() < 256 &&
				meshShaderSrc.find(".glsl") != -1)
			{
				meshShaderSource = ReadFile(meshShaderSrc);
			}
			else {
				meshShaderSource = meshShaderSrc;
			}
			GLuint meshShader = glCreateShader(GL_MESH_SHADER_NV);
			compileShader(meshShader, meshShaderSource.c_str());
			glDeleteShader(meshShader);
		}

		OpenGLMeshShader::~OpenGLMeshShader()
		{
			glDeleteProgram(m_RendererID);
		}
		void OpenGLMeshShader::Bind() const
		{
			glUseProgram(m_RendererID);
		}
		void OpenGLMeshShader::Unbind() const
		{
			glUseProgram(0);
		}

		void OpenGLMeshShader::SetInt(const std::string& name, int value)
		{
			UploadUniformInt(name, value);
		}

		void OpenGLMeshShader::SetIntArray(const std::string& name, int* values, uint32_t count)
		{
			UploadUniformIntArray(name, values, count);
		}
		void OpenGLMeshShader::SetUIntArray(const std::string& name, uint32_t* values, uint32_t count)
		{
			UploadUniformUIntArray(name, values, count);
		}

		void OpenGLMeshShader::SetFloat(const std::string& name, float value)
		{
			UploadUniformFloat(name, value);
		}

		void OpenGLMeshShader::SetFloatArray(const std::string& name, float* values, uint32_t count)
		{
			UploadUniformFloatArray(name,values,count);
		}

		void OpenGLMeshShader::SetFloat2(const std::string& name, const glm::vec2& value)
		{
			UploadUniformFloat2(name, value);
		}

		void OpenGLMeshShader::SetFloat3(const std::string& name, const glm::vec3& value)
		{
			UploadUniformFloat3(name, value);
		}

		void OpenGLMeshShader::SetFloat4(const std::string& name, const glm::vec4& value)
		{
			UploadUniformFloat4(name, value);
		}

		void OpenGLMeshShader::SetMat4(const std::string& name, const glm::mat4& value)
		{
			UploadUniformMat4(name, value);
		}

		void OpenGLMeshShader::UploadUniformInt(const std::string& name, int value)
		{
			GLint location = GetUniformLocation(name);
			glUniform1i(location, value);
		}

		void OpenGLMeshShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
		{
			GLint location = GetUniformLocation(name);
			glUniform1iv(location, count, values);
		}

		void OpenGLMeshShader::UploadUniformUIntArray(const std::string& name, uint32_t* values, uint32_t count)
		{
			GLint location = GetUniformLocation(name);
			glUniform1uiv(location, count, values);
		}

		void OpenGLMeshShader::UploadUniformFloat(const std::string& name, float value)
		{
			GLint location = GetUniformLocation(name);
			glUniform1f(location, value);
		}

		void OpenGLMeshShader::UploadUniformFloatArray(const std::string& name, float* values, uint32_t count)
		{
			GLint location = GetUniformLocation(name);
			glUniform1fv(location, count, values);
		}

		void OpenGLMeshShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
		{
			GLint location = GetUniformLocation(name);
			glUniform2f(location, value.x, value.y);
		}

		void OpenGLMeshShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
		{
			GLint location = GetUniformLocation(name);
			glUniform3f(location, value.x, value.y, value.z);
		}

		void OpenGLMeshShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
		{
			GLint location = GetUniformLocation(name);
			glUniform4f(location, value.x, value.y, value.z, value.w);
		}

		void OpenGLMeshShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
		{
			GLint location = GetUniformLocation(name);
			glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
		}

		void OpenGLMeshShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
		{
			GLint location = GetUniformLocation(name);
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
		}

		std::string OpenGLMeshShader::ReadFile(const std::string& filepath)
		{
			return std::string();
		}

		int OpenGLMeshShader::GetUniformLocation(const std::string& name, bool block)
		{
			if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
				return m_UniformLocationCache[name];
			int location;
			if (block)
			{
				location = glGetUniformBlockIndex(m_RendererID, name.c_str());
				glUniformBlockBinding(m_RendererID, location, 0);
			}
			else
				location = glGetUniformLocation(m_RendererID, name.c_str());
			if (location == -1)
				Kiana_CORE_WARN("{0} uniform use {1} is null", GetName(), name);
			m_UniformLocationCache[name] = location;
			return location;
		}

		void OpenGLMeshShader::GenComputeResultBuffer(uint32_t size)
		{
		}

		void OpenGLMeshShader::StartCompute(uint32_t x, uint32_t y, uint32_t z)
		{
		}

		void OpenGLMeshShader::GetComputeResult(void* t)
		{
		}

		void OpenGLMeshShader::SetName(std::string name)
		{
			m_Name = name;
		}

		void OpenGLMeshShader::SetMat4s(const std::string& name, std::map<uint32_t, glm::mat4>& mat4s)
		{
		}

	}
}