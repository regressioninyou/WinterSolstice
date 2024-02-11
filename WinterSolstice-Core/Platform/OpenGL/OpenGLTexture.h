#pragma once
#include "WinterSolstice/Render/Texture.h"
typedef unsigned int GLenum;
namespace WinterSolstice {
	namespace Bronya {

		class OpenGLTexture2D : public Texture2D
		{
		public:
			OpenGLTexture2D(const TextureSpecification& specification);
			OpenGLTexture2D(const std::string& path);
			virtual ~OpenGLTexture2D();

			virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

			virtual uint32_t GetWidth() const override { return m_Width; }
			virtual uint32_t GetHeight() const override { return m_Height; }
			virtual uint32_t GetRendererID() const override { return m_RendererID; }
			virtual void SetRendererID(uint32_t id) override { m_RendererID = id; };

			virtual const std::string& GetPath() const override { return m_Path; }

			virtual void SetData(void* data, uint32_t size) override;

			virtual void Bind(uint32_t slot = 0) const override;
			virtual void UnBind(uint32_t slot = 0) const override;

			virtual const std::string& getName()override { return name; };
			virtual void setName(std::string target)override { name = target; };
			virtual const std::string& getType()override { return type; };
			virtual void setType(std::string target) override { type = target; };
			virtual const std::string& getPath()override { return path; };
			virtual void setPath(std::string target) override { path = target; };
			virtual bool isTranslucent() override { return translucent; }
			virtual void SetTranslucent(bool set)override { translucent = set; }
			virtual void SetUse(bool set)override { use = set; }
			virtual void SetShaderUsing(uintptr_t index, bool set = false)override 
			{ 
				ShaderUsingMap[index] = set;
				int i = 0;
			}
			virtual bool IsUse() override { return use && BuildState; }
			virtual bool IsShaderUsing(uintptr_t index)override { if (ShaderUsingMap.find(index) == ShaderUsingMap.end()) return false; return ShaderUsingMap[index] && BuildState; }
			virtual bool isBuildState()override { return BuildState; }
			virtual void SetBuildState(bool set)override { BuildState = set; }

			virtual bool operator==(const Texture& other) const override
			{
				return m_RendererID == other.GetRendererID();
			}
		private:
			TextureSpecification m_Specification;

			std::string m_Path;
			uint32_t m_Width, m_Height;
			uint32_t m_RendererID;
			GLenum m_InternalFormat, m_DataFormat;


			std::string name;
			std::string type;
			std::string path;
			bool use = false;
			bool translucent = false;
			bool BuildState = false;
			std::unordered_map<uintptr_t, bool> ShaderUsingMap;
		};

		class OpenGLFrameBufferTexture : public Texture2D {
		public:
			OpenGLFrameBufferTexture(uint32_t width = 1024,uint32_t height = 1024);
			~OpenGLFrameBufferTexture();

			// 通过 Texture2D 继承
			const TextureSpecification& GetSpecification() const override;

			uint32_t GetWidth() const override;
			uint32_t GetHeight() const override;
			uint32_t GetRendererID() const override;

			void SetRendererID(uint32_t id) override;
			const std::string& GetPath() const override;
			void SetData(void* data, uint32_t size) override;

			void Bind(uint32_t slot) const override;
			void UnBind(uint32_t slot) const override;

			bool operator==(const Texture& other) const override;

			const std::string& getName() override;
			void setName(std::string target) override;
			const std::string& getType() override;
			void setType(std::string target) override;
			const std::string& getPath() override;
			void setPath(std::string target) override;
			bool isTranslucent() override;
			void SetTranslucent(bool set) override;
			void SetUse(bool set) override;
			void SetShaderUsing(uintptr_t index, bool set) override;
			bool IsUse() override;
			bool IsShaderUsing(uintptr_t index) override;
			bool isBuildState() override;
			void SetBuildState(bool set) override;
		private:
			uint32_t m_RendererID;
			uint32_t m_Width, m_Height;
			GLenum m_InternalFormat, m_DataFormat;
		};
		class OpenGLTextureCube :public TextureCube
		{
		public:
			OpenGLTextureCube(const TextureSpecification& specification);
			OpenGLTextureCube(const std::string& path);

			virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

			virtual uint32_t GetWidth() const override { return m_Width; }
			virtual uint32_t GetHeight() const override { return m_Height; }
			virtual uint32_t GetRendererID() const override { return m_RendererID; }
			virtual void SetRendererID(uint32_t id) override { m_RendererID = id; };

			virtual const std::string& GetPath() const override { return m_Path; }

			virtual void SetData(void* data, uint32_t size) override;

			virtual void Bind(uint32_t slot) const override;
			virtual void UnBind(uint32_t slot) const override;

			virtual bool operator==(const Texture& other) const override
			{
				return m_RendererID == other.GetRendererID();
			}

			virtual const std::string& getName()override { return name; };
			virtual void setName(std::string target)override { name = target; };
			virtual const std::string& getType()override { return type; };
			virtual void setType(std::string target) override { type = target; };
			virtual const std::string& getPath()override { return path; };
			virtual void setPath(std::string target) override { path = target; };
			virtual bool isTranslucent() override { return translucent; }
			virtual void SetTranslucent(bool set)override { translucent = set; }
			virtual void SetUse(bool set)override { use = set; }
			virtual void SetShaderUsing(uintptr_t index, bool set = false)override { ShaderUsingMap[index] = set; }
			virtual bool IsUse() override { return use && BuildState; }
			virtual bool IsShaderUsing(uintptr_t index)override { if (ShaderUsingMap.find(index) == ShaderUsingMap.end()) return false; return ShaderUsingMap[index] && BuildState; }
			virtual bool isBuildState()override { return BuildState; }
			virtual void SetBuildState(bool set)override { BuildState = set; }

		private:
			TextureSpecification m_Specification;

			std::string m_Path;
			uint32_t m_Width, m_Height;
			uint32_t m_RendererID;
			GLenum m_InternalFormat, m_DataFormat;


			std::string name;
			std::string type;
			std::string path;
			bool use = false;
			bool translucent = false;
			bool BuildState = false;
			std::unordered_map<uintptr_t, bool> ShaderUsingMap;
		};
	}
}
