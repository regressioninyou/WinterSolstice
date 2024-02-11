#pragma once 
#include "KyBao/Render/Texture.h"
namespace KyBao {
	class SkyBox
	{
	public:
		SkyBox(const std::string& SkyBoxPath);


	private:
		Ref<TextureCube> cubeMap;
	};
}