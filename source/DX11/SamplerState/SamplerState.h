#ifndef DX11_SAMPLER_STATE_H
#define DX11_SAMPLER_STATE_H

#include "Framework/DX11.Framework.h"
#include <d3d11.h>
#include <d3dcompiler.h>


class SamplerState {
public:

	SamplerState() = default;
	SamplerState(const D3D11_SAMPLER_DESC& desc);
	~SamplerState();

	HRESULT Create(const D3D11_SAMPLER_DESC& desc);

	static SamplerState CreateLinearWrap();
	static SamplerState CreatePointClamp();
	static SamplerState CreateAnisotropic(UINT maxAnisotropy = 16);

	void BindToVS(UINT slot);
	void BindToHS(UINT slot);
	void BindToDS(UINT slot);
	void BindToGS(UINT slot);
	void BindToPS(UINT slot);
	void BindToAll(UINT slot);
private:
	ID3D11SamplerState* pSampler = nullptr;

};



#endif