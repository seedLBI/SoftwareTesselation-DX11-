#ifndef DX11_TEXTURE_2D_H
#define DX11_TEXTURE_2D_H

#include "Framework/DX11.Framework.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>




class Texture2D {

public:
	Texture2D() = default;
	Texture2D(const std::string& path2file, int count_desired_channels = 4);
	~Texture2D();

	void LoadTextureFromFile(const std::string& path2file, int count_desired_channels = 4);

	void BindToVS(UINT slot);
	void BindToHS(UINT slot);
	void BindToDS(UINT slot);
	void BindToGS(UINT slot);
	void BindToPS(UINT slot);
	void BindToAll(UINT slot);

private:
	ID3D11Texture2D* tex = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;

};






#endif