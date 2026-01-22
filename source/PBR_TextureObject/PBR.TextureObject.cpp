#include "PBR.TextureObject.h"

PBR_TextureObject::PBR_TextureObject() {

}

PBR_TextureObject::~PBR_TextureObject() {

}

void PBR_TextureObject::LoadFromFolder(const std::string& path) {

	std::filesystem::path fs_path(path);

	for (const auto& entry : std::filesystem::recursive_directory_iterator(fs_path)) {

		std::string path2file = entry.path().string();
		std::string name = entry.path().stem().generic_string();

		if (name == "albedo") {
			meterials.albedo.LoadTextureFromFile(path2file);
		}
		else if (name == "ao") {
			meterials.ao.LoadTextureFromFile(path2file);
		}
		else if (name == "displacement") {
			meterials.displacement.LoadTextureFromFile(path2file);
		}
		else if (name == "metallic") {
			meterials.metallic.LoadTextureFromFile(path2file);
		}
		else if (name == "normal") {
			meterials.normal.LoadTextureFromFile(path2file);
		}
		else if (name == "opacity") {
			meterials.opacity.LoadTextureFromFile(path2file);
		}
		else if (name == "roughness") {
			meterials.roughness.LoadTextureFromFile(path2file);
		}
	}


}

void PBR_TextureObject::BindToAll() {
	meterials.albedo.BindToPS(0);
	meterials.ao.BindToPS(1);
	meterials.displacement.BindToVS(2);
	meterials.metallic.BindToPS(3);
	meterials.opacity.BindToPS(4);
	meterials.roughness.BindToPS(5);
	meterials.normal.BindToPS(6);

}