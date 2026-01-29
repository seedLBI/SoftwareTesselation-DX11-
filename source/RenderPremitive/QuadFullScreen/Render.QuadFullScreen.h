#ifndef RENDER_QUAD_FULLSCREEN_H
#define RENDER_QUAD_FULLSCREEN_H

#include "Framework/DX11.Framework.h"
#include "DX11/Shader/DX11.Shader.h"


#include <DirectXMath.h>

class Render_QuadFullscreen{
public:
	Render_QuadFullscreen(DX11_Shader& shader);
	~Render_QuadFullscreen();

	void Render();

private:
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pIndexBuffer;

	struct VertexData { 
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT2 Texcoord;
	};

	void Init(DX11_Shader& shader);
	void InitInputLayout(DX11_Shader& shader);
	void InitVertexBuffer();
	void InitIndexBuffer();
};




#endif