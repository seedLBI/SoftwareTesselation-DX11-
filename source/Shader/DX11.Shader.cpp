#include "DX11.Shader.h"
#include <iostream>

void PrintCompileErrors(ID3DBlob* errorBlob)
{
	if (!errorBlob) return;
	const char* msg = static_cast<const char*>(errorBlob->GetBufferPointer());
	if (msg) std::cout << msg;
}

HRESULT DX11_Shader::CompileShaderFromFile(
	const std::wstring& filePath,
	const char* entryPoint,
	const char* target,
	ComPtr<ID3DBlob>& outBlob,
	UINT compileFlags)
{
	outBlob.Reset();
	ComPtr<ID3DBlob> errorBlob;
	#ifdef _DEBUG
		compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif
	HRESULT hr = D3DCompileFromFile(
		filePath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,
		target,
		compileFlags,
		0,
		outBlob.GetAddressOf(),
		errorBlob.GetAddressOf());
	if (FAILED(hr)) {
		PrintCompileErrors(errorBlob.Get());
	}
	return hr;
}


HRESULT DX11_Shader::LoadShadersFromFile(
    const std::wstring& filePath,
	const char* vsEntry, const char* psEntry,
    const char* vsTarget, const char* psTarget)
{
    HRESULT hr = S_OK;

    // Vertex
    if (vsEntry)
    {
        ComPtr<ID3DBlob> vsBlob;
        hr = CompileShaderFromFile(filePath, vsEntry, vsTarget, vsBlob);
        if (FAILED(hr)) {

            return hr;
        }
		
        hr = framework::directx::GetDevice()->CreateVertexShader(
			vsBlob->GetBufferPointer(), 
			vsBlob->GetBufferSize(), 
			nullptr, 
			vertexShader.GetAddressOf()
		);

        if (FAILED(hr)) return hr;
        vsBytecode = vsBlob;
    }

    // Pixel
    if (psEntry)
    {
		
        ComPtr<ID3DBlob> psBlob;
        hr = CompileShaderFromFile(filePath, psEntry, psTarget, psBlob);
        if (FAILED(hr)) return hr;
        hr = framework::directx::GetDevice()->CreatePixelShader(
			psBlob->GetBufferPointer(), 
			psBlob->GetBufferSize(), 
			nullptr, 
			pixelShader.GetAddressOf()
		);

        if (FAILED(hr)) return hr;
        psBytecode = psBlob;
    }

    return hr;
}


HRESULT DX11_Shader::LoadBlobFromFile(
    const std::wstring& path, 
    ComPtr<ID3DBlob>& outBlob)
{
    outBlob.Reset();

    FILE* f = nullptr;
#ifdef _WIN32
    _wfopen_s(&f, path.c_str(), L"rb");
#else
    f = fopen(std::string(path.begin(), path.end()).c_str(), "rb");
#endif
    if (!f) return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size <= 0) { fclose(f); return E_FAIL; }
    void* data = malloc(size);
    if (!data) { fclose(f); return E_OUTOFMEMORY; }
    fread(data, 1, size, f);
    fclose(f);



    ID3DBlob* blob = nullptr;
    HRESULT hr = D3DCreateBlob(size, &blob);
    if (FAILED(hr)) { free(data); return hr; }
    memcpy(blob->GetBufferPointer(), data, size);
    free(data);
    outBlob = blob;
    blob->Release();
    return S_OK;
}

HRESULT DX11_Shader::CreateInputLayout(
    const D3D11_INPUT_ELEMENT_DESC* desc, 
    UINT numElements)
{
	if (!vsBytecode) return E_FAIL;

	HRESULT hr = framework::directx::GetDevice()->CreateInputLayout(
        desc, numElements, 
        vsBytecode->GetBufferPointer(), 
        vsBytecode->GetBufferSize(), 
        inputLayout.GetAddressOf()
    );

	return hr;
}



HRESULT DX11_Shader::CreateConstantBuffer(
    const std::string& name, 
    UINT bindSlot, 
    UINT sizeBytes)
{
    UINT aligned = (sizeBytes + 15) & ~15U;
    if (cbuffers.find(name) != cbuffers.end()) return E_INVALIDARG;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = aligned;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ComPtr<ID3D11Buffer> buf;
    HRESULT hr = framework::directx::GetDevice()->CreateBuffer(
        &bd, 
        nullptr, 
        buf.GetAddressOf()
    );

    if (FAILED(hr)) return hr;

    CBufferBinding binding;
    binding.buffer = buf;
    binding.bindSlot = bindSlot;
    binding.size = aligned;
    cbuffers[name] = binding;
    return S_OK;
}


HRESULT DX11_Shader::UpdateConstantBuffer(
    const std::string& name, 
    const void* data, 
    UINT sizeBytes)
{
    auto it = cbuffers.find(name);
    if (it == cbuffers.end()) return E_FAIL;
    CBufferBinding& b = it->second;
    if (sizeBytes > b.size) return E_FAIL;
    D3D11_MAPPED_SUBRESOURCE mapped;

    HRESULT hr = framework::directx::GetContext()->Map(
        b.buffer.Get(), 
        0, 
        D3D11_MAP_WRITE_DISCARD, 
        0, 
        &mapped
    );

    if (FAILED(hr)) return hr;
    memcpy(mapped.pData, data, sizeBytes);

    framework::directx::GetContext()->Unmap(
        b.buffer.Get(), 
        0
    );

    return S_OK;
}


void DX11_Shader::BindConstantBufferToVS(const std::string& name)
{
    auto it = cbuffers.find(name);
    if (it == cbuffers.end()) return;
    CBufferBinding& b = it->second;
    framework::directx::GetContext()->VSSetConstantBuffers(
        b.bindSlot, 
        1, 
        b.buffer.GetAddressOf()
    );

}
void DX11_Shader::BindConstantBufferToPS(const std::string& name)
{
    auto it = cbuffers.find(name);
    if (it == cbuffers.end()) return;
    CBufferBinding& b = it->second;
    framework::directx::GetContext()->PSSetConstantBuffers(
        b.bindSlot, 
        1, 
        b.buffer.GetAddressOf()
    );

}

void DX11_Shader::PSSetShaderResource(UINT slot, ID3D11ShaderResourceView* srv)
{
    framework::directx::GetContext()->PSSetShaderResources(slot, 1, &srv);
}
void DX11_Shader::PSSetSampler(UINT slot, ID3D11SamplerState* sampler)
{
    framework::directx::GetContext()->PSSetSamplers(slot, 1, &sampler);
}
void DX11_Shader::Bind()
{
    framework::directx::GetContext()->IASetInputLayout(inputLayout.Get());
    framework::directx::GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
    framework::directx::GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);
    framework::directx::GetContext()->GSSetShader(nullptr, nullptr, 0);
    framework::directx::GetContext()->CSSetShader(nullptr, nullptr, 0);
}

void DX11_Shader::Unbind()
{
    ID3D11VertexShader* nullVS = nullptr;
    ID3D11PixelShader* nullPS = nullptr;
    ID3D11GeometryShader* nullGS = nullptr;
    framework::directx::GetContext()->VSSetShader(nullVS, nullptr, 0);
    framework::directx::GetContext()->PSSetShader(nullPS, nullptr, 0);
    framework::directx::GetContext()->GSSetShader(nullGS, nullptr, 0);
}

ID3D11VertexShader* DX11_Shader::GetVS() const 
{ 
    return vertexShader.Get(); 
}

ID3D11PixelShader* DX11_Shader::GetPS() const 
{ 
    return pixelShader.Get(); 
}

ID3D11InputLayout* DX11_Shader::GetInputLayout() const 
{ 
    return inputLayout.Get(); 
}