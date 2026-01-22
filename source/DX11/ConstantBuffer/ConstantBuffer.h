#ifndef DX11_CONSTANT_BUFFER_H
#define DX11_CONSTANT_BUFFER_H

#include "Framework/DX11.Framework.h"
#include <d3d11.h>
#include <d3dcompiler.h>

template <typename T>
struct ConstantBuffer {
public:
	ConstantBuffer(T&, UINT slot = 0);
	ConstantBuffer(UINT slot = 0);
	~ConstantBuffer();
	
	void SetBindSlot(UINT slot);

	void Update(T&);

	void BindToVS();
	void BindToHS();
	void BindToDS();
	void BindToGS();
	void BindToPS();
	void BindToAll();

	void BindToHS(UINT slot);
	void BindToVS(UINT slot);
	void BindToDS(UINT slot);
	void BindToGS(UINT slot);
	void BindToPS(UINT slot);
	void BindToAll(UINT slot);

private:
	ID3D11Buffer* buffer = 0;
	UINT bindSlot;

	void InitBuffer();
	void InitBuffer(T&);

	static UINT Align16(UINT size) { return (size + 15u) & ~15u; }
};


template <typename T>
ConstantBuffer<T>::ConstantBuffer(UINT slot) {
	InitBuffer();
	this->bindSlot = slot;
}

template <typename T>
ConstantBuffer<T>::ConstantBuffer(T& value, UINT slot) {
	InitBuffer(value);
	this->bindSlot = slot;
}

template <typename T>
ConstantBuffer<T>::~ConstantBuffer() {
	if (buffer)
		delete buffer;
}

template <typename T>
void ConstantBuffer<T>::SetBindSlot(UINT slot) {
	bindSlot = slot;
}


template <typename T>
void ConstantBuffer<T>::InitBuffer(T& value) {
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.ByteWidth = Align16(sizeof(T));
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = &value;

	auto device = framework::directx::GetDevice();

	device->CreateBuffer(
		&bd,
		nullptr,
		&buffer);
}

template <typename T>
void ConstantBuffer<T>::InitBuffer() {
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.ByteWidth = Align16(sizeof(T));
	bd.StructureByteStride = 0;


	auto device = framework::directx::GetDevice();

	device->CreateBuffer(
		&bd,
		nullptr,
		&buffer);

}

template <typename T>
void ConstantBuffer<T>::Update(T& value) {
	D3D11_MAPPED_SUBRESOURCE mapped;

	framework::directx::GetContext()->Map(
		buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped
	);

	memcpy(mapped.pData, &value, sizeof(T));

	framework::directx::GetContext()->Unmap(
		buffer,
		0
	);
}



template <typename T>
void ConstantBuffer<T>::BindToVS() {

	BindToVS(bindSlot);

}
template <typename T>
void ConstantBuffer<T>::BindToVS(UINT slot) {
	framework::directx::GetContext()->VSSetConstantBuffers(
		slot,
		1,
		&buffer
	);
}

template <typename T>
void ConstantBuffer<T>::BindToHS() {
	BindToHS(bindSlot);
}
template <typename T>
void ConstantBuffer<T>::BindToHS(UINT slot) {
	framework::directx::GetContext()->HSSetConstantBuffers(
		slot,
		1,
		&buffer
	);
}

template <typename T>
void ConstantBuffer<T>::BindToDS() {
	BindToDS(bindSlot);
}
template <typename T>
void ConstantBuffer<T>::BindToDS(UINT slot) {
	framework::directx::GetContext()->DSSetConstantBuffers(
		slot,
		1,
		&buffer
	);
}


template <typename T>
void ConstantBuffer<T>::BindToGS() {
	BindToGS(bindSlot);
}
template <typename T>
void ConstantBuffer<T>::BindToGS(UINT slot) {

	framework::directx::GetContext()->GSSetConstantBuffers(
		slot,
		1,
		&buffer
	);

}


template <typename T>
void ConstantBuffer<T>::BindToPS() {
	BindToPS(bindSlot);
}
template <typename T>
void ConstantBuffer<T>::BindToPS(UINT slot) {
	framework::directx::GetContext()->PSSetConstantBuffers(
		slot,
		1,
		&buffer
	);
}

template <typename T>
void ConstantBuffer<T>::BindToAll() {
	BindToVS();
	BindToHS();
	BindToDS();
	BindToGS();
	BindToPS();
}
template <typename T>
void ConstantBuffer<T>::BindToAll(UINT slot) {
	BindToVS(slot);
	BindToHS(slot);
	BindToDS(slot);
	BindToGS(slot);
	BindToPS(slot);
}


#endif