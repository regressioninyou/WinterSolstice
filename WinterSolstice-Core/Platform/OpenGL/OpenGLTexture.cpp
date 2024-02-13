#include "k423.h"
#include "Platform/OpenGL/OpenGLTexture.h"

#include "Platform/load/stb_image/stb_image.h"
#include <glad/glad.h>
#include "WinterSolstice/Core/Application.h"

namespace WinterSolstice {
	namespace Bronya {
#define Test 1
		namespace Utils {

			static GLenum KianaImageFormatToGLDataFormat(ImageFormat format)
			{
				switch (format)
				{
				case ImageFormat::RGB8:  return GL_RGB;
				case ImageFormat::RGBA8: return GL_RGBA;
				}

				Kiana_CORE_ASSERT(false);
				return 0;
			}

			static GLenum KianaImageFormatToGLInternalFormat(ImageFormat format)
			{
				switch (format)
				{
				case ImageFormat::RGB8:  return GL_RGB8;
				case ImageFormat::RGBA8: return GL_RGBA8;
				}
				Kiana_CORE_ASSERT(false);
				return 0;
			}

		}
		OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
			: m_Specification(specification), m_Width(m_Specification.Width), m_Height(m_Specification.Height)
		{
			std::string Name = specification.Name;
			setName(Name);
			std::string Path = specification.Path;
			setPath(Path);
			m_InternalFormat = Utils::KianaImageFormatToGLInternalFormat(m_Specification.Format);
			m_DataFormat = Utils::KianaImageFormatToGLDataFormat(m_Specification.Format);

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
		OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
			: m_Path(path)
		{
			int width, height, channels;
			//stbi_set_flip_vertically_on_load(1);
			stbi_uc* data = nullptr;
			{
				data = stbi_load(path.c_str(), &width, &height, &channels, 0);
			}

			if (data)
			{
				auto lasSlash = path.find_last_of("/\\");
				lasSlash = lasSlash == std::string::npos ? 0 : lasSlash + 1;
				auto lastDot = path.rfind('.');
				auto count = lastDot == std::string::npos ? path.size() - lasSlash : lastDot - lasSlash;
				std::string name = path.substr(lasSlash, count);
				setName(name);
				setPath(path);

				m_Width = width;
				m_Height = height;

				GLenum internalFormat = 0, dataFormat = 0;
				if (channels == 4)
				{
					internalFormat = GL_RGBA8;
					dataFormat = GL_RGBA;
				}
				else if (channels == 3)
				{
					internalFormat = GL_RGB8;
					dataFormat = GL_RGB;
				}

				m_InternalFormat = internalFormat;
				m_DataFormat = dataFormat;

				Kiana_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");
#if Test
				glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
				glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

				glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
#else
				glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
				glBindTexture(GL_TEXTURE_2D, m_RendererID);
				glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#endif

				stbi_image_free(data);
				BuildState = true;
			}
			else {
				Kiana_CORE_WARN("Texture : {0} loader 失败", path.c_str());
				SetUse(false);
				BuildState = false;
			}
		}

		OpenGLTexture2D::~OpenGLTexture2D()
		{
			glDeleteTextures(1, &m_RendererID);
		}

		void OpenGLTexture2D::SetData(void* data, uint32_t size)
		{
			uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
			Kiana_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
		}

		void OpenGLTexture2D::Bind(uint32_t slot) const
		{
#if Test
			glBindTextureUnit(slot, m_RendererID);
#else
			glBindTexture(GL_TEXTURE_2D + slot, m_RendererID);
#endif
		}

		void OpenGLTexture2D::UnBind(uint32_t slot) const
		{
#if !Test
			glBindTexture(GL_TEXTURE_2D + slot, 0);
#endif
		}

		OpenGLTextureCube::OpenGLTextureCube(const TextureSpecification& specification)
			: m_Specification(specification), m_Width(m_Specification.Width), m_Height(m_Specification.Height)
		{
		}

		OpenGLTextureCube::OpenGLTextureCube(const std::string& path)
		{
			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
			std::vector<std::string> faces{
				path + "/right.jpg",
				path + "/left.jpg",
				path + "/top.jpg",
				path + "/bottom.jpg",
				path + "/front.jpg",
				path + "/back.jpg"
			};
			int width, height, nrChannels;
			for (unsigned int i = 0; i < 6; i++)
			{
				unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
					);
					stbi_image_free(data);
				}
				else
				{
					std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
					stbi_image_free(data);
				}
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		void OpenGLTextureCube::SetData(void* data, uint32_t size)
		{
		}
		void OpenGLTextureCube::Bind(uint32_t slot) const
		{
			glBindTextureUnit(slot, m_RendererID);
		}
		void OpenGLTextureCube::UnBind(uint32_t slot) const
		{
		}
		void BindTextureID(uint32_t renderId, uint32_t slot)
		{
			glBindTextureUnit(slot, renderId);
		}
		void UnBindTextureID(uint32_t renderId, uint32_t slot)
		{
			glBindTextureUnit(slot, 0);
		}
		OpenGLFrameBufferTexture::OpenGLFrameBufferTexture(uint32_t width, uint32_t height)
			:m_Width(width), m_Height(height)
		{
		}
		OpenGLFrameBufferTexture::~OpenGLFrameBufferTexture()
		{
		}
		const TextureSpecification& OpenGLFrameBufferTexture::GetSpecification() const
		{
			// TODO: 在此处插入 return 语句
			return {};
		}
		uint32_t OpenGLFrameBufferTexture::GetWidth() const
		{
			return m_Width;
		}
		uint32_t OpenGLFrameBufferTexture::GetHeight() const
		{
			return m_Height;
		}
		uint32_t OpenGLFrameBufferTexture::GetRendererID() const
		{
			return m_RendererID;
		}
		void OpenGLFrameBufferTexture::SetRendererID(uint32_t id)
		{
			m_RendererID = id;
		}
		const std::string& OpenGLFrameBufferTexture::GetPath() const
		{
			// TODO: 在此处插入 return 语句
			return "texture";
		}
		void OpenGLFrameBufferTexture::SetData(void* data, uint32_t size)
		{
		}
		void OpenGLFrameBufferTexture::Bind(uint32_t slot) const
		{
			glBindTextureUnit(slot, m_RendererID);
		}
		void OpenGLFrameBufferTexture::UnBind(uint32_t slot) const
		{
		}
		bool OpenGLFrameBufferTexture::operator==(const Texture& other) const
		{
			return false;
		}
		const std::string& OpenGLFrameBufferTexture::getName()
		{
			// TODO: 在此处插入 return 语句
			return "texture";
		}
		void OpenGLFrameBufferTexture::setName(std::string target)
		{
		}
		const std::string& OpenGLFrameBufferTexture::getType()
		{
			// TODO: 在此处插入 return 语句
			return "texture";
		}
		void OpenGLFrameBufferTexture::setType(std::string target)
		{
		}
		const std::string& OpenGLFrameBufferTexture::getPath()
		{
			// TODO: 在此处插入 return 语句
			return "texture";
		}
		void OpenGLFrameBufferTexture::setPath(std::string target)
		{
		}
		bool OpenGLFrameBufferTexture::isTranslucent()
		{
			return false;
		}
		void OpenGLFrameBufferTexture::SetTranslucent(bool set)
		{
		}
		void OpenGLFrameBufferTexture::SetUse(bool set)
		{
		}
		void OpenGLFrameBufferTexture::SetShaderUsing(uintptr_t index, bool set)
		{
		}
		bool OpenGLFrameBufferTexture::IsUse()
		{
			return false;
		}
		bool OpenGLFrameBufferTexture::IsShaderUsing(uintptr_t index)
		{
			return false;
		}
		bool OpenGLFrameBufferTexture::isBuildState()
		{
			return false;
		}
		void OpenGLFrameBufferTexture::SetBuildState(bool set)
		{
		}
	}
}
