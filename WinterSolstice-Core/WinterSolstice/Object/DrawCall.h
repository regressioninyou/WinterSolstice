#include "k423.h"
#include "WinterSolstice/Render/Shader.h"
#include "WinterSolstice/Render/Texture.h"
#include <map>
#include <array>
#include <WinterSolstice/Render/VertexArray.h>
#define TextureMax 4
#define CASTUINTPTR(x) (uintptr_t)(x)
#define FILLFLOAT32 {-1, -1, -1, -1,-1, -1, -1, -1,-1, -1, -1, -1,-1, -1, -1, -1,-1, -1, -1, -1,-1, -1, -1, -1,-1, -1, -1, -1,-1, -1, -1, -1};
namespace WinterSolstice {
	class DrawCall
	{
	public:
		DrawCall() {
			diffuseNr.first = "u_texture_Diffuse";
			specularNr.first = "u_texture_Specular";
			normalNr.first = "u_texture_Normals";
			heightNr.first = "u_texture_Height";
			std::fill(u_TilingFactors.begin(), u_TilingFactors.end(), -1);
			this_ptr = (uintptr_t)this;
		}
		virtual void BindTexture(Ref<Bronya::Shader> shader) {
			if (ready) {
				unsigned int textureIndex = 0;
				for (auto& texture : textures) {
					if (texture.second->IsShaderUsing(this_ptr)) {
						texture.second->Bind(textureIndex);
						textureIndex++;
					}
				}
				if (textureIndex == 0) {
					ready = false;
				}
			}
			if (init && !ready) return;

			unsigned int textureIndex = 0;
			int diff = 0;
			int spec = 0;
			int norm = 0;
			int heig = 0;

			u_TilingFactors = FILLFLOAT32;
			diffuseNr.second = { -1, -1 ,-1 ,-1 };
			specularNr.second = { -1, -1 ,-1 ,-1 };
			normalNr.second = { -1, -1 ,-1 ,-1 };
			heightNr.second = { -1, -1 ,-1 ,-1 };
			for (auto& texture : textures) {
				if (texture.second->IsShaderUsing(this_ptr)) {
					name = texture.second->getType();
					if (name == "Diffuse" && diff < TextureMax)
					{
						diffuseNr.second[diff++] = textureIndex;
					}
					else if (name == "Specular" && spec < TextureMax)
					{
						specularNr.second[spec++] = textureIndex; // transfer uint32_t to string
					}
					else if (name == "Normals" && norm < TextureMax)
					{
						normalNr.second[norm++] = textureIndex;// transfer uint32_t to string
					}
					else if (name == "Height" && heig < TextureMax)
					{
						heightNr.second[heig++] = textureIndex; // transfer uint32_t to string
					}
					else {
						continue;
					}
					u_TilingFactors[textureIndex] = TilingFactors[texture.first];
					textureIndex++;
				}
			}
			shader->SetIntArray(diffuseNr.first, diffuseNr.second.data(), TextureMax);
			shader->SetIntArray(specularNr.first, specularNr.second.data(), TextureMax);
			shader->SetIntArray(normalNr.first, normalNr.second.data(), TextureMax);
			shader->SetIntArray(heightNr.first, heightNr.second.data(), TextureMax);
			shader->SetFloatArray("u_TilingFactor", u_TilingFactors.data(), 32);
			init = true;
		};
		virtual void addTexture(std::string name, Ref<Bronya::Texture2D> tex) {
			if (textures.find(tex->getName()) != textures.end()) return;
			if (tex->isTranslucent())
				translucent = true;
			tex->SetShaderUsing(CASTUINTPTR(this), true);
			textures[name] = tex;
			TilingFactors[name] = 1.0f;
			init = false;
			ready = true;
		}
		virtual void addTexture(std::pair<std::string, Ref<Bronya::Texture2D>> tex) {
			if (textures.find(tex.second->getName()) != textures.end()) return;
			if (tex.second->isTranslucent())
				translucent = true;
			tex.second->SetShaderUsing(CASTUINTPTR(this), true);
			textures[tex.first] = tex.second;
			TilingFactors[tex.first] = 1.0f;
			init = false;
			ready = true;
		}
		virtual void addTexture(Ref<Bronya::Texture2D> tex) {
			if (textures.find(tex->getName()) != textures.end()) return;
			if (tex->isTranslucent())
				translucent = true;
			tex->SetShaderUsing(CASTUINTPTR(this), true);
			textures[tex->getName()] = tex;
			TilingFactors[tex->getName()] = 1.0f;
			init = false;
			ready = true;
		}
		virtual void ResetTexture(Ref<Bronya::Texture2D> target, Ref<Bronya::Texture2D> reset) {
			init = false;
			ready = true;
			if (!reset->isBuildState()) {
				Kiana_CORE_WARN("Reset不是一个有效的纹理");
				return;
			}
			if (!target->isBuildState()) {
				target->SetRendererID(reset->GetRendererID());
				reset->SetRendererID(0);
				target->SetBuildState(true);
				return;
			}
			auto it = textures.find(target->getName());
			if (it == textures.end()) {
				addTexture(reset);
				return;
			}
			auto resetIt = textures.find(reset->getName());
			if (resetIt == textures.end()) {
				it->second = reset;

			}
			else {
				it->second = reset;
				textures.erase(resetIt);
			}
		}
		virtual bool isTranslucent()
		{
			return translucent;
		}
		virtual void SetTranslucent(bool set) {
			translucent = set;
			init = false;
		}
		virtual Ref<Bronya::Texture2D> getTexture(std::string name) {
			if (textures.find(name) == textures.end())
			{
				return nullptr;
			}
			return textures[name];
		}
		virtual std::unordered_map<std::string, Ref<Bronya::Texture2D>> getTexture() {
			return textures;
		}
		virtual void setVertexArray(Ref<Bronya::VertexArray> va) {
			m_VertexArray = va;
			init = false;
		}
		virtual Ref<Bronya::VertexArray> getVertexArray() {
			return m_VertexArray;
		}
		virtual bool isReady() {
			return ready || AlwaysRunning;
		}
		virtual void SetAlwaysRunning(bool set) {
			AlwaysRunning = set;
			init = false;
			ready = true;
		}
		virtual bool GetAlawysRunning() {
			return AlwaysRunning;
		}

		virtual void SetTilingFactor(const std::string& texture, float TilingFactor) {
			if (TilingFactors.find(texture) != TilingFactors.end()) {
				init = false;
				ready = true;
				TilingFactors[texture] = TilingFactor;
			}
		}
		virtual float GetTilingFactor(const std::string& texture) {
			if (TilingFactors.find(texture) != TilingFactors.end()) {
				return TilingFactors[texture];
			}
			return -1.0f;
		}
		virtual void SetUsingTexture(Ref<Bronya::Texture2D> texture, bool set) {
			if (textures.find(texture->getName()) != textures.end()) {
				init = false;
				ready = true;
				texture->SetShaderUsing(this_ptr, set);
			}
		}
		virtual bool isUsingTexture(Ref<Bronya::Texture2D> texture) {
			auto it = textures.find(texture->getName());
			if (it != textures.end()) {
				init = false;
				ready = true;
				return it->second->IsShaderUsing(this_ptr);
			}
			Kiana_CORE_WARN("{0} Not Fund this here",texture->getName());
			return false;
		}
		inline auto begin() { return textures.begin(); }
		inline auto end() { return textures.end(); }
	protected:
		uintptr_t this_ptr;
		std::string number;
		std::string name;
		bool ready = false;
		bool AlwaysRunning = false;
		bool translucent = false;
		bool init = false;
		Ref<Bronya::VertexArray> m_VertexArray;
		std::unordered_map<std::string, Ref<Bronya::Texture2D>> textures;
		std::unordered_map<std::string, float> TilingFactors;
		std::array<float, 32> u_TilingFactors;

	protected:
		std::pair<std::string, std::array<int, TextureMax>> diffuseNr;
		std::pair<std::string, std::array<int, TextureMax>> specularNr;
		std::pair<std::string, std::array<int, TextureMax>> normalNr;
		std::pair<std::string, std::array<int, TextureMax>> heightNr;
	};
}