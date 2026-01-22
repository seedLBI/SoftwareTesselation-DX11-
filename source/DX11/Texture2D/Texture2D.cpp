#include "Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

Texture2D::Texture2D(const std::string& path2file, int count_desired_channels) {
	LoadTextureFromFile(path2file, count_desired_channels);
}

Texture2D::~Texture2D() {

}

void Texture2D::LoadTextureFromFile(const std::string& path2file, int count_desired_channels) {

	std::cout << "\tTexture2D::LoadTextureFromFile\n";

	int width, height, channels;
	//stbi_set_flip_vertically_on_load(true);
	uint8_t* pixels = stbi_load(path2file.c_str(), &width, &height, &channels, 4);

	std::cout << "\t==TEXTURE INFO==\n";
	std::cout << "\t\twidth:    " << width << std::endl;
	std::cout << "\t\theight:   " << height << std::endl;
	std::cout << "\t\tchannels: " << channels << std::endl;


	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = pixels;
	initData.SysMemPitch = width * 4;
	initData.SysMemSlicePitch = 0;

	HRESULT hr = framework::directx::GetDevice()->CreateTexture2D(&desc, &initData, &tex);
	if (FAILED(hr)) {
		std::cout << "ERROR CreateTexture2D\n";
		exit(-1111);
	}

	hr = framework::directx::GetDevice()->CreateShaderResourceView(tex, nullptr, &srv);
	if (FAILED(hr)) {
		std::cout << "ERROR CreateShaderResourceView\n";
		exit(-1111);
	}

	stbi_image_free(pixels);

}


void Texture2D::BindToHS(UINT slot) {
	framework::directx::GetContext()->HSSetShaderResources(slot, 1, &srv);
}

void Texture2D::BindToVS(UINT slot) {
	framework::directx::GetContext()->VSSetShaderResources(slot, 1, &srv);
}

void Texture2D::BindToDS(UINT slot) {
	framework::directx::GetContext()->DSSetShaderResources(slot, 1, &srv);
}

void Texture2D::BindToGS(UINT slot) {
	framework::directx::GetContext()->GSSetShaderResources(slot, 1, &srv);
}

void Texture2D::BindToPS(UINT slot) {
	framework::directx::GetContext()->PSSetShaderResources(slot, 1, &srv);
}

void Texture2D::BindToAll(UINT slot) {
	BindToVS(slot);
	BindToHS(slot);
	BindToDS(slot);
	BindToGS(slot);
	BindToPS(slot);
}