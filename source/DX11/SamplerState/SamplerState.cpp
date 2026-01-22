#include "SamplerState.h"


SamplerState::SamplerState(const D3D11_SAMPLER_DESC& desc) {
	Create(desc);
}

SamplerState::~SamplerState() {
	//pSampler->Release();
}


HRESULT SamplerState::Create(const D3D11_SAMPLER_DESC& desc) {
	HRESULT hr = framework::directx::GetDevice()->CreateSamplerState(&desc, &pSampler);
	return hr;
}


SamplerState SamplerState::CreateLinearWrap()
{
	D3D11_SAMPLER_DESC d = {};
	d.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d.MipLODBias = 0.0f;
	d.MaxAnisotropy = 1;
	d.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	d.BorderColor[0] = d.BorderColor[1] = d.BorderColor[2] = d.BorderColor[3] = 0.0f;
	d.MinLOD = 0;
	d.MaxLOD = D3D11_FLOAT32_MAX;

	return SamplerState(d);
}

SamplerState SamplerState::CreatePointClamp()
{
	D3D11_SAMPLER_DESC d = {};
	d.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	d.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d.MipLODBias = 0.0f;
	d.MaxAnisotropy = 1;
	d.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	d.BorderColor[0] = d.BorderColor[1] = d.BorderColor[2] = d.BorderColor[3] = 0.0f;
	d.MinLOD = 0;
	d.MaxLOD = D3D11_FLOAT32_MAX;

	return SamplerState(d);
}

SamplerState SamplerState::CreateAnisotropic(UINT maxAnisotropy)
{
	D3D11_SAMPLER_DESC d = {};
	d.Filter = D3D11_FILTER_ANISOTROPIC;
	d.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d.MipLODBias = 0.0f;
	d.MaxAnisotropy = maxAnisotropy > 0 ? maxAnisotropy : 1;
	d.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	d.BorderColor[0] = d.BorderColor[1] = d.BorderColor[2] = d.BorderColor[3] = 0.0f;
	d.MinLOD = 0;
	d.MaxLOD = D3D11_FLOAT32_MAX;

	return SamplerState(d);
}



void SamplerState::BindToVS(UINT slot) {
	framework::directx::GetContext()->VSSetSamplers(slot, 1, &pSampler);
}
void SamplerState::BindToHS(UINT slot) {
	framework::directx::GetContext()->HSSetSamplers(slot, 1, &pSampler);
}
void SamplerState::BindToDS(UINT slot) {
	framework::directx::GetContext()->DSSetSamplers(slot, 1, &pSampler);
}
void SamplerState::BindToGS(UINT slot) {
	framework::directx::GetContext()->GSSetSamplers(slot, 1, &pSampler);
}
void SamplerState::BindToPS(UINT slot) {
	framework::directx::GetContext()->PSSetSamplers(slot, 1, &pSampler);
}
void SamplerState::BindToAll(UINT slot) {
	BindToVS(slot);
	BindToHS(slot);
	BindToDS(slot);
	BindToGS(slot);
	BindToPS(slot);
}