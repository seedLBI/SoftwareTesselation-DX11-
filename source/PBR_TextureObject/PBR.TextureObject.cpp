#include "PBR.TextureObject.h"

#include <iostream>

PBR_TextureObject::PBR_TextureObject() {

}

PBR_TextureObject::~PBR_TextureObject() {

}

void PBR_TextureObject::LoadFromFolder(const std::string& path) {

	std::filesystem::path fs_path(path);
	std::cout << "[PBR][" << fs_path.filename().string() << "]---------------\n";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(fs_path)) {



		std::string path2file = entry.path().string();
		std::string name = entry.path().stem().generic_string();


		if (name == "albedo") {
			std::cout << "\t[ALBEDO]       ";
			meterials.albedo.LoadTextureFromFile(path2file);
		}
		else if (name == "ao") {
			std::cout << "\t[AO]           ";
			meterials.ao.LoadTextureFromFile(path2file);
		}
		else if (name == "displacement") {
			std::cout << "\t[DISPLACEMENT] ";

			meterials.displacement.LoadTextureFromFile(path2file);
		}
		else if (name == "metallic") {
			std::cout << "\t[METALLIC]     ";

			meterials.metallic.LoadTextureFromFile(path2file);
		}
		else if (name == "normal") {
			std::cout << "\t[NORMAL]       ";

			meterials.normal.LoadTextureFromFile(path2file);
		}
		else if (name == "opacity") {
			std::cout << "\t[OPACITY]      ";

			meterials.opacity.LoadTextureFromFile(path2file);
		}
		else if (name == "roughness") {
			std::cout << "\t[ROUGHNESS]    ";

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