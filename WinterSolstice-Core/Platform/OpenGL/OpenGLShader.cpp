#include "k423.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <WinterSolstice/Core/Time.h>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>


namespace WinterSolstice {
	namespace Bronya {
		namespace Utils {

			static GLenum ShaderTypeFromString(const std::string& type)
			{
				if (type == "vertex")
					return GL_VERTEX_SHADER;
				if (type == "fragment" || type == "pixel")
					return GL_FRAGMENT_SHADER;

				Kiana_CORE_ASSERT(false, "Unknown shader type!");
				return 0;
			}

			static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
			{
				switch (stage)
				{
				case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
				case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
				}
				Kiana_CORE_ASSERT(false);
				return (shaderc_shader_kind)0;
			}

			static const char* GLShaderStageToString(GLenum stage)
			{
				switch (stage)
				{
				case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
				case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
				}
				Kiana_CORE_ASSERT(false);
				return nullptr;
			}

			static const char* GetCacheDirectory()
			{
				// TODO: make sure the assets directory is valid
				return "assets/cache/shader/opengl";
			}

			static void CreateCacheDirectoryIfNeeded()
			{
				std::string cacheDirectory = GetCacheDirectory();
				if (!std::filesystem::exists(cacheDirectory))
					std::filesystem::create_directories(cacheDirectory);
			}

			static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
			{
				switch (stage)
				{
				case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
				case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
				}
				Kiana_CORE_ASSERT(false);
				return "";
			}

			static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
			{
				switch (stage)
				{
				case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
				case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
				}
				Kiana_CORE_ASSERT(false);
				return "";
			}


		}


		OpenGLShader::OpenGLShader()
		{
		}

		OpenGLShader::OpenGLShader(const std::string& filepath)
			:m_FilePath(filepath)
		{
			Utils::CreateCacheDirectoryIfNeeded();

			std::string source = ReadFile(filepath);
			auto shaderSources = PreProcess(source);

			{
				Kiana::Timer timer;
				CompileOrGetVulkanBinaries(shaderSources);
				CompileOrGetOpenGLBinaries();
				CreateProgram();
				Kiana_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
			}

			// Extract name from filepath
			auto lastSlash = filepath.find_last_of("/\\");
			lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
			auto lastDot = filepath.rfind('.');
			auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
			m_Name = filepath.substr(lastSlash, count);
		}
		OpenGLShader::OpenGLShader(const std::string& vs, const std::string& fs)
		{
			Kiana::Timer timer;
			std::string vsSource, fsSource;
			if (vs.size() < 256 &&
				fs.size() < 256 &&
				vs.find(".glsl") != -1 &&
				fs.find(".glsl") != -1)
			{
				vsSource = ReadFile(vs);
				fsSource = ReadFile(fs);
				auto lasSlash = vs.find_last_of("/\\");
				lasSlash = lasSlash == std::string::npos ? 0 : lasSlash + 1;
				auto lastDot = vs.rfind('_');
				auto count = lastDot == std::string::npos ? vs.size() - lasSlash : lastDot - lasSlash;
				m_Name = vs.substr(lasSlash, count);
			}
			else {
				vsSource = vs;
				fsSource = fs;
				m_Name = "defult shader";
			}

			std::unordered_map<GLenum, std::string> sources;
			sources[GL_VERTEX_SHADER] = vsSource;
			sources[GL_FRAGMENT_SHADER] = fsSource;
			Compile(sources);
			Kiana_CORE_WARN("{0} Shader creation took {1} ms", m_Name.c_str(), timer.ElapsedMillis());
			//CompileOrGetVulkanBinaries(sources);
			//CompileOrGetOpenGLBinaries();
			//CreateProgram();
		}

		OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
			: m_Name(name)
		{
			std::unordered_map<GLenum, std::string> sources;
			sources[GL_VERTEX_SHADER] = vertexSrc;
			sources[GL_FRAGMENT_SHADER] = fragmentSrc;

			//CompileOrGetVulkanBinaries(sources);
			//CompileOrGetOpenGLBinaries();
			Compile(sources);
		}

		OpenGLShader::~OpenGLShader()
		{
			glDeleteProgram(m_RendererID);
		}

		std::string OpenGLShader::ReadFile(const std::string& filepath)
		{
			std::string result;
			std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
			if (in)
			{
				in.seekg(0, std::ios::end);
				size_t size = in.tellg();
				if (size != -1)
				{
					result.resize(size);
					in.seekg(0, std::ios::beg);
					in.read(&result[0], size);
				}
				else
				{
					Kiana_CORE_ERROR("Could not read from file '{0}'", filepath);
				}
			}
			else
			{
				Kiana_CORE_ERROR("Could not open file '{0}'", filepath);
			}

			return result;
		}
		std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
		{
			std::unordered_map<GLenum, std::string> shaderSources;

			const char* typeToken = "#type";
			size_t typeTokenLength = strlen(typeToken);
			size_t pos = source.find(typeToken, 0);
			while (pos != std::string::npos)
			{
				size_t eol = source.find_first_of("\r\n", pos);
				Kiana_CORE_ASSERT(eol != std::string::npos, "Syntax error");
				size_t begin = pos + typeTokenLength + 1;
				std::string type = source.substr(begin, eol - begin);
				Kiana_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified");

				size_t nextLinePos = source.find_first_not_of("\r\n", eol);
				pos = source.find(typeToken, nextLinePos);
				shaderSources[Utils::ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
			}

			return shaderSources;
		}



		void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
		{
			GLuint program = glCreateProgram();
			std::vector<GLenum> glShaderIDs(shaderSources.size());
			for (auto& kv : shaderSources)
			{
				GLenum type = kv.first;
				const std::string& source = kv.second;

				GLuint shader = glCreateShader(type);

				const GLchar* sourceCStr = source.c_str();
				glShaderSource(shader, 1, &sourceCStr, 0);

				glCompileShader(shader);

				GLint isCompiled = 0;
				glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
				if (isCompiled == GL_FALSE)
				{
					GLint maxLength = 0;
					glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

					std::vector<GLchar> infoLog(maxLength);
					glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

					glDeleteShader(shader);

					Kiana_CORE_ERROR("{0}", infoLog.data());
					Kiana_CORE_ASSERT(false, "Shader compilation failure!");
					break;
				}

				glAttachShader(program, shader);
				glShaderIDs.push_back(shader);
			}

			m_RendererID = program;

			// Link our program
			glLinkProgram(program);

			// Note the different functions here: glGetProgram* instead of glGetShader*.
			GLint isLinked = 0;
			glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
			if (isLinked == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

				// We don't need the program anymore.
				glDeleteProgram(program);

				for (auto id : glShaderIDs)
					glDeleteShader(id);

				Kiana_CORE_ERROR("{0}", infoLog.data());
				Kiana_CORE_ASSERT(false, "Shader link failure!");
				return;
			}

			for (auto id : glShaderIDs)
				glDetachShader(program, id);
		}

		void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
		{
			if (m_FilePath.empty()) {
				Compile(shaderSources);
				return;
			}
			GLuint program = glCreateProgram();

			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
			const bool optimize = true;
			if (optimize)
				options.SetOptimizationLevel(shaderc_optimization_level_performance);

			std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

			auto& shaderData = m_VulkanSPIRV;
			shaderData.clear();
			for (auto&& [stage, source] : shaderSources)
			{
				std::filesystem::path shaderFilePath = m_FilePath;
				std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

				std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
				if (in.is_open())
				{
					in.seekg(0, std::ios::end);
					auto size = in.tellg();
					in.seekg(0, std::ios::beg);

					auto& data = shaderData[stage];
					data.resize(size / sizeof(uint32_t));
					in.read((char*)data.data(), size);
				}
				else
				{
					shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
					if (module.GetCompilationStatus() != shaderc_compilation_status_success)
					{
						Kiana_CORE_ERROR(module.GetErrorMessage());
						Kiana_CORE_ASSERT(false);
					}

					shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

					std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
					if (out.is_open())
					{
						auto& data = shaderData[stage];
						out.write((char*)data.data(), data.size() * sizeof(uint32_t));
						out.flush();
						out.close();
					}
				}
			}

			for (auto&& [stage, data] : shaderData)
				Reflect(stage, data);
		}

		void OpenGLShader::CompileOrGetOpenGLBinaries()
		{
			if (m_FilePath.empty()) return;
			auto& shaderData = m_OpenGLSPIRV;

			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
			const bool optimize = false;
			if (optimize)
				options.SetOptimizationLevel(shaderc_optimization_level_performance);

			std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

			shaderData.clear();
			m_OpenGLSourceCode.clear();
			for (auto&& [stage, spirv] : m_VulkanSPIRV)
			{
				std::filesystem::path shaderFilePath = m_FilePath;
				std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

				std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
				if (in.is_open())
				{
					in.seekg(0, std::ios::end);
					auto size = in.tellg();
					in.seekg(0, std::ios::beg);

					auto& data = shaderData[stage];
					data.resize(size / sizeof(uint32_t));
					in.read((char*)data.data(), size);
				}
				else
				{
					spirv_cross::CompilerGLSL glslCompiler(spirv);
					m_OpenGLSourceCode[stage] = glslCompiler.compile();
					auto& source = m_OpenGLSourceCode[stage];

					shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str());
					if (module.GetCompilationStatus() != shaderc_compilation_status_success)
					{
						Kiana_CORE_ERROR(module.GetErrorMessage());
						Kiana_CORE_ASSERT(false);
					}

					shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

					std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
					if (out.is_open())
					{
						auto& data = shaderData[stage];
						out.write((char*)data.data(), data.size() * sizeof(uint32_t));
						out.flush();
						out.close();
					}
				}
			}
		}

		void OpenGLShader::CreateProgram()
		{
			if (m_FilePath.empty()) return;
			GLuint program = glCreateProgram();

			std::vector<GLuint> shaderIDs;
			for (auto&& [stage, spirv] : m_OpenGLSPIRV)
			{
				GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
				glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
				glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
				glAttachShader(program, shaderID);
			}

			glLinkProgram(program);

			GLint isLinked;
			glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
			if (isLinked == GL_FALSE)
			{
				GLint maxLength;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
				Kiana_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, infoLog.data());

				glDeleteProgram(program);

				for (auto id : shaderIDs)
					glDeleteShader(id);
			}

			for (auto id : shaderIDs)
			{
				glDetachShader(program, id);
				glDeleteShader(id);
			}

			m_RendererID = program;
		}

		void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
		{
			spirv_cross::Compiler compiler(shaderData);
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();

			Kiana_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
			Kiana_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
			Kiana_CORE_TRACE("    {0} resources", resources.sampled_images.size());

			Kiana_CORE_TRACE("Uniform buffers:");
			for (const auto& resource : resources.uniform_buffers)
			{
				const auto& bufferType = compiler.get_type(resource.base_type_id);
				uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
				uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				int memberCount = bufferType.member_types.size();

				Kiana_CORE_TRACE("  {0}", resource.name);
				Kiana_CORE_TRACE("    Size = {0}", bufferSize);
				Kiana_CORE_TRACE("    Binding = {0}", binding);
				Kiana_CORE_TRACE("    Members = {0}", memberCount);
			}
		}

		void OpenGLShader::Bind() const
		{
			glUseProgram(m_RendererID);
		}

		int OpenGLShader::GetUniformLocation(const std::string& name, bool block) {
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

		void OpenGLShader::Unbind() const
		{
			glUseProgram(0);
		}

		void OpenGLShader::SetInt(const std::string& name, int value)
		{
			UploadUniformInt(name, value);
		}

		void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
		{
			UploadUniformIntArray(name, values, count);
		}
		void OpenGLShader::SetUIntArray(const std::string& name, uint32_t* values, uint32_t count)
		{
			UploadUniformUIntArray(name, values, count);
		}

		void OpenGLShader::SetFloat(const std::string& name, float value)
		{
			UploadUniformFloat(name, value);
		}

		void OpenGLShader::SetFloatArray(const std::string& name, float* values, uint32_t count)
		{
			UploadUniformFloatArray(name, values, count);
		}

		void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
		{
			UploadUniformFloat2(name, value);
		}

		void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
		{
			UploadUniformFloat3(name, value);
		}

		void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
		{
			UploadUniformFloat4(name, value);
		}

		void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
		{
			UploadUniformMat4(name, value);
		}

		void OpenGLShader::SetMat4s(const std::string& name, std::map<uint32_t, glm::mat4>& mat4s)
		{
			UploadUniformMat4s(name, mat4s);
		}

		void OpenGLShader::SetName(std::string name)
		{
			m_Name = name;
		}

		void OpenGLShader::UploadUniformInt(const std::string& name, int value)
		{
			GLint location = GetUniformLocation(name);
			glUniform1i(location, value);
		}

		void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
		{
			GLint location = GetUniformLocation(name);
			glUniform1iv(location, count, values);
		}
		void OpenGLShader::UploadUniformUIntArray(const std::string& name, uint32_t* values, uint32_t count)
		{
			GLint location = GetUniformLocation(name);
			glUniform1uiv(location, count, values);
		}

		void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
		{
			GLint location = GetUniformLocation(name);
			glUniform1f(location, value);
		}

		void OpenGLShader::UploadUniformFloatArray(const std::string& name, float* values, uint32_t count)
		{
			GLint location = GetUniformLocation(name);
			glUniform1fv(location, count, values);
		}

		void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
		{
			GLint location = GetUniformLocation(name);
			glUniform2f(location, value.x, value.y);
		}

		void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
		{
			GLint location = GetUniformLocation(name);
			glUniform3f(location, value.x, value.y, value.z);
		}

		void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
		{
			GLint location = GetUniformLocation(name);
			glUniform4f(location, value.x, value.y, value.z, value.w);
		}

		void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
		{
			GLint location = GetUniformLocation(name);
			glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
		}

		void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
		{
			GLint location = GetUniformLocation(name);
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
		}
		void OpenGLShader::UploadUniformMat4s(const std::string& name, std::map<uint32_t, glm::mat4>& mat4s)
		{
			GLint location = GetUniformLocation(name);
			std::vector<GLfloat> matrixData;
			for (const auto& entry : mat4s) {
				const glm::mat4& matrix = entry.second;
				matrixData.insert(matrixData.end(), glm::value_ptr(matrix), glm::value_ptr(matrix) + 16);
			}
			//for (auto& pair : mat4s) {
			glUniformMatrix4fv(location, matrixData.size() / 16, GL_FALSE, matrixData.data());
			//}
		}

		OpenGLComputeShader::OpenGLComputeShader(const std::string& filepath)
		{
			auto buf = ReadFile(filepath);
			auto lasSlash = filepath.find_last_of("/\\");
			lasSlash = lasSlash == std::string::npos ? 0 : lasSlash + 1;
			auto lastDot = filepath.rfind('.');
			auto count = lastDot == std::string::npos ? filepath.size() - lasSlash : lastDot - lasSlash;
			SetName(filepath.substr(lasSlash, count));

			std::unordered_map<GLenum, std::string> sources;
			sources[GL_COMPUTE_SHADER] = buf;
			Compile(sources);

		}

		void OpenGLComputeShader::GenComputeResultBuffer(uint32_t size)
		{
			// 创建结果缓冲
			glGenBuffers(1, &resultBuffer);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, resultBuffer);
			glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_COPY);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, resultBuffer);
			resultBufferSize = size;
		}

		void OpenGLComputeShader::StartCompute(uint32_t x, uint32_t y, uint32_t z)
		{
			glDispatchCompute(x, y, z);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}

		void OpenGLComputeShader::GetComputeResult(void* t)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, resultBuffer);
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, resultBufferSize, t);
			glDeleteBuffers(1, &resultBuffer);
		}

	}
}
