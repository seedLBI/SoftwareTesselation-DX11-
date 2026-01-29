#ifndef DX11_TEXTURE_2D_H
#define DX11_TEXTURE_2D_H

#include "Framework/DX11.Framework.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <memory>



class Texture2D {

public:
	Texture2D() = default;
	Texture2D(const std::string& path2file, int count_desired_channels = 4);
	~Texture2D();
	void Release();


	void CreateEmpty(
		UINT width, UINT height,
		UINT bindFlags = D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D11_USAGE usage = D3D11_USAGE_DEFAULT,
		UINT cpuAccessFlags = 0,
		UINT mipLevels = 1,
		UINT arraySize = 1);

	void LoadTextureFromFile(const std::string& path2file, int count_desired_channels = 4);

	void BindToVS(UINT slot);
	void BindToHS(UINT slot);
	void BindToDS(UINT slot);
	void BindToGS(UINT slot);
	void BindToPS(UINT slot);
	void BindToAll(UINT slot);

	ID3D11ShaderResourceView* GetSRV() const;
	ID3D11Texture2D* GetTexture() const;

private:
	ID3D11Texture2D* tex = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;


	UINT bindFlag;
	bool CheckBindFlag(UINT flag);

};






#endif