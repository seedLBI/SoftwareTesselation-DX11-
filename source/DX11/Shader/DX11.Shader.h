#ifndef DX11_SHADER_H
#define DX11_SHADER_H

#include "Framework/DX11.Framework.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <unordered_map>


using Microsoft::WRL::ComPtr;

void PrintCompileErrors(ID3DBlob* errorBlob);

class DX11_Shader {
public:
	DX11_Shader() = default;
	~DX11_Shader() = default;
	

	static HRESULT CompileShaderFromFile(
		const std::wstring& filePath,
		const char* entryPoint,
		const char* target,
		ComPtr<ID3DBlob>& outBlob,
		UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS
	);


	HRESULT LoadShadersFromFile(
		const std::wstring& filePath,
		const char* vsEntry = "VSMain",
		const char* psEntry = "PSMain",
		const char* vsTarget = "vs_5_0",
		const char* psTarget = "ps_5_0"
	);


	HRESULT LoadShadersFromFile_withGS(
		const std::wstring& filePath,
		const char* vsEntry = "VSMain",
		const char* gsEntry = "GSMain",
		const char* psEntry = "PSMain",
		const char* vsTarget = "vs_5_0",
		const char* gsTarget = "gs_5_0",
		const char* psTarget = "ps_5_0"
	);


	static HRESULT LoadBlobFromFile(
		const std::wstring& path, 
		ComPtr<ID3DBlob>& outBlob
	);


	HRESULT CreateInputLayout(
		const D3D11_INPUT_ELEMENT_DESC* desc,
		UINT numElements
	);



	void PSSetShaderResource(UINT slot, ID3D11ShaderResourceView* srv);
	void PSSetSampler(UINT slot, ID3D11SamplerState* sampler);
	void Bind();
	void Unbind();


	ID3D11VertexShader* GetVS() const;
	ID3D11PixelShader* GetPS() const;
	ID3D11InputLayout* GetInputLayout() const;

private:

	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11GeometryShader> geometryShader;
	ComPtr<ID3D11PixelShader> pixelShader;


	ComPtr <ID3DBlob> vsBytecode;
	ComPtr <ID3DBlob> gsBytecode;
	ComPtr <ID3DBlob> psBytecode;
	
	ComPtr <ID3D11InputLayout> inputLayout;

};



#endif