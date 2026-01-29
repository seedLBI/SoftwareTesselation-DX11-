#include "Render.QuadFullScreen.h"


void Render_QuadFullscreen::Init(DX11_Shader& shader) {
	InitInputLayout(shader);
	InitVertexBuffer();
	InitIndexBuffer();
}

void Render_QuadFullscreen::InitInputLayout(DX11_Shader& shader) {
	using namespace framework::directx;

	D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	shader.CreateInputLayout(inputLayoutDesc, 2);
	inputLayout = shader.GetInputLayout();
}
void Render_QuadFullscreen::InitVertexBuffer() {
	using namespace framework::directx;

	VertexData vertices[] = {
		{{-1.f,-1.f,0.f}, {0.f,0.f}}, // LB
		{{-1.f,1.f,0.f},  {0.f,1.f}}, // LT
		{{1.f,1.f,0.f},   {1.f,1.f}}, // RT
		{{1.f,-1.f,0.f},  {1.f,0.f}}  // RB
	};

	D3D11_BUFFER_DESC bd_v;
	bd_v.Usage = D3D11_USAGE_DEFAULT;
	bd_v.ByteWidth = sizeof(vertices);
	bd_v.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_v.CPUAccessFlags = 0;
	bd_v.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData_v = {};
	InitData_v.pSysMem = vertices;

	GetDevice()->CreateBuffer(&bd_v, &InitData_v, &pVertexBuffer);
}
void Render_QuadFullscreen::InitIndexBuffer() {
	using namespace framework::directx;

	UINT indices[] = {
		0,1,2,
		0,2,3
	};

	D3D11_BUFFER_DESC bd_i;
	bd_i.Usage = D3D11_USAGE_DEFAULT;
	bd_i.ByteWidth = sizeof(indices);
	bd_i.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd_i.CPUAccessFlags = 0;
	bd_i.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData_i = {};
	InitData_i.pSysMem = indices;

	GetDevice()->CreateBuffer(&bd_i, &InitData_i, &pIndexBuffer);
}


Render_QuadFullscreen::Render_QuadFullscreen(DX11_Shader& shader) {
	Init(shader);
}

Render_QuadFullscreen::~Render_QuadFullscreen() {

}

void Render_QuadFullscreen::Render() {
	using namespace framework::directx;

	GetContext()->IASetInputLayout(inputLayout);

	UINT stride = sizeof(VertexData);
	UINT offset = 0;
	GetContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	GetContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	GetContext()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	GetContext()->DrawIndexed(6, 0, 0);
}