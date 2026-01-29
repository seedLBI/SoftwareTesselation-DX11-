#ifndef REDNER_TEXTURE_2D_H
#define RENDER_TEXTURE_2D_H


#include "DX11/Texture2D/Texture2D.h"
#include <DirectXMath.h>
#include <memory>


class RenderTexture2D {
public:
	RenderTexture2D(UINT width, UINT height);
	~RenderTexture2D();

	void SetRenderTarget();
	void ClearRenderTarget(DirectX::XMFLOAT4 rgba);

private:

	void Init(UINT width, UINT height);



	Texture2D textureRender;
	ID3D11RenderTargetView* m_renderTargetView;
	void InitRenderTarget(UINT width, UINT height);



	Texture2D textureDepth;
	ID3D11DepthStencilView* m_depthStencilView;
	void InitDepthStencil(UINT width, UINT height);



	D3D11_VIEWPORT m_viewport;
	void InitViewport(UINT width, UINT height);

};


#endif