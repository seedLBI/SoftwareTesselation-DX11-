#ifndef PBR_TEXTURE_OBJECT_H
#define PBR_TEXTURE_OBJECT_H

#include "DX11/Texture2D/Texture2D.h"

#include <string>
#include <filesystem>

class PBR_TextureObject {
public:
	PBR_TextureObject();
	~PBR_TextureObject();

	void LoadFromFolder(const std::string& path);

	void BindToAll();

private:
	struct MaterialData {
		Texture2D albedo;
		Texture2D ao;
		Texture2D displacement;
		Texture2D metallic;
		Texture2D opacity;
		Texture2D roughness;
		Texture2D normal;
	};

	MaterialData meterials;
};




#endif