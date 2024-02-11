#pragma once 
namespace WinterSolstice {
	namespace Bronya {
		enum class ImageFormat
		{
			None = 0,
			R8,
			RGB8,
			RGBA8,
			RGBA32F
		};

		struct TextureSpecification
		{
			std::string Path;
			std::string Name;
			uint32_t Width = 1;
			uint32_t Height = 1;
			ImageFormat Format = ImageFormat::RGBA8;
			bool GenerateMips = true;
		};

		class Texture
		{
		public:
			virtual ~Texture() = default;

			virtual const TextureSpecification& GetSpecification() const = 0;

			virtual uint32_t GetWidth() const = 0;
			virtual uint32_t GetHeight() const = 0;
			virtual uint32_t GetRendererID() const = 0;
			virtual void SetRendererID(uint32_t id) = 0;

			virtual const std::string& GetPath() const = 0;

			virtual void SetData(void* data, uint32_t size) = 0;

			virtual void Bind(uint32_t slot = 0) const = 0;
			virtual void UnBind(uint32_t slot = 0) const = 0;

			virtual bool operator==(const Texture& other) const = 0;

			virtual const std::string& getName() = 0;
			virtual void setName(std::string target) = 0;
			virtual const std::string& getType() = 0;
			virtual void setType(std::string target) = 0;
			virtual const std::string& getPath() = 0;
			virtual void setPath(std::string target) = 0;
			virtual bool isTranslucent() = 0;
			virtual void SetTranslucent(bool set) = 0;
			virtual void SetUse(bool set) = 0;
			virtual void SetShaderUsing(uintptr_t index, bool set = false) = 0;
			virtual bool IsUse() = 0;
			virtual bool IsShaderUsing(uintptr_t index) = 0;
			virtual bool isBuildState() = 0;
			virtual void SetBuildState(bool set) = 0;
		};
		class Texture2D : public Texture
		{
		public:
			static Ref<Texture2D> Create(const TextureSpecification& specification);
			static Ref<Texture2D> Create(const std::string& path);
			static Ref<Texture2D> CreateFrameTexture(uint32_t width,uint32_t height);
		};
		class TextureCube :public Texture {
		public:
			static Ref<TextureCube> CreateCubeMap(const std::string& path);
		};
	}
}