#include "RenderTexture2D.h"


void RenderTexture2D::Init(UINT width, UINT height) {
	InitRenderTarget(width, height);
	InitDepthStencil(width, height);
	InitViewport(width, height);
}

void RenderTexture2D::InitRenderTarget(UINT width, UINT height) {
	textureRender.CreateEmpty(
		width,
		height,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		DXGI_FORMAT_R8G8B8A8_UNORM
	);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	HRESULT hr = framework::directx::GetDevice()->CreateRenderTargetView(
		textureRender.GetTexture(),
		&renderTargetViewDesc,
		&m_renderTargetView);

	if (FAILED(hr)) {
		exit(10101);
	}

}

void RenderTexture2D::InitDepthStencil(UINT width, UINT height) {
	textureDepth.CreateEmpty(
		width,
		height,
		D3D11_BIND_DEPTH_STENCIL,
		DXGI_FORMAT_D24_UNORM_S8_UINT
	);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HRESULT hr = framework::directx::GetDevice()->CreateDepthStencilView(
		textureDepth.GetTexture(),
		&depthStencilViewDesc,
		&m_depthStencilView
	);

	if (FAILED(hr)) {
		exit(10102);
	}


}

void RenderTexture2D::InitViewport(UINT width, UINT height) {
	m_viewport.Width = (float)width;
	m_viewport.Height = (float)height;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
}

void RenderTexture2D::ClearRenderTarget(DirectX::XMFLOAT4 rgba) {
	float color[4];

	color[0] = rgba.x;
	color[1] = rgba.y;
	color[2] = rgba.z;
	color[3] = rgba.w;

	framework::directx::GetContext()->ClearRenderTargetView(m_renderTargetView, color);
	framework::directx::GetContext()->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void RenderTexture2D::SetRenderTarget() {
	framework::directx::GetContext()->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	framework::directx::GetContext()->RSSetViewports(1, &m_viewport);
}

RenderTexture2D::RenderTexture2D(UINT width, UINT height) {
	Init(width, height);

}

RenderTexture2D::~RenderTexture2D() {

}
