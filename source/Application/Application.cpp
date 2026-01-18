#include "Application.h"

#include <iostream>

Application::Application(HINSTANCE hInstance, int nCmdShow) {
	framework::core::Init(hInstance, nCmdShow);

	framework::time::SetFPS(0);
	framework::directx::EnableVSync();

	shader.LoadShadersFromFile(L"resources\\shaders\\shader_software_tesselation.hlsl");
	shader.CreateConstantBuffer("MatrixView", 0, sizeof(cbuffer_MatrixView));
	shader.CreateConstantBuffer("Indexes", 1, sizeof(cbuffer_Indexes));
	shader.CreateConstantBuffer("Time", 2, sizeof(cbuffer_Time));
	shader.CreateConstantBuffer("LightDirectional", 3, sizeof(cbuffer_LightDirectional));
	shader.BindConstantBufferToVS("MatrixView");
	shader.BindConstantBufferToVS("Indexes");
	shader.BindConstantBufferToVS("Time");

	shader.BindConstantBufferToPS("MatrixView");
	shader.BindConstantBufferToPS("LightDirectional");


	current_Subdivision = 24;

	UpdateSubdivision();


}

Application::~Application() {
	framework::core::Terminate();
}



void Application::Frame() {
	using namespace framework::directx;

	GetContext()->IASetInputLayout(nullptr);
	UINT stride = 0;
	UINT offset = 0;
	ID3D11Buffer* nullVB[1] = { nullptr };
	GetContext()->IASetVertexBuffers(0, 0, nullptr, &stride, &offset);
	GetContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/*
	XMMATRIX wvp = XMMatrixOrthographicOffCenterLH(
		-2.0f, 2.0f,   // left, right
		-2.0f, 2.0f,   // bottom, top 
		  0.0f,  1.0f    // near, far
	);
	*/

	auto size = framework::window::GetSize();

	float aspect = float(size.x) / float(size.y);


	float r_fov = XMConvertToRadians(camera.GetFOV());

	XMMATRIX proj = XMMatrixPerspectiveFovLH(r_fov, aspect, 0.01f, 100.f);
	XMMATRIX view = camera.getMatrixView();

	XMMATRIX VP = view * proj;


	XMStoreFloat4x4(&MVP.wvp, XMMatrixTranspose(VP));
	XMStoreFloat3(&MVP.posView, camera.GetPos());

	shader.UpdateConstantBuffer("MatrixView", &MVP, sizeof(MVP));
	
	TIME.time = framework::time::GetTimeProgram();
	shader.UpdateConstantBuffer("Time", &TIME, sizeof(TIME));
	


	float time = framework::time::GetTimeProgram();

	float azimuthSpeed = 2.5f;
	float elevationSpeed = 2.3f;

	float azimuth = time * azimuthSpeed;
	float elevation = sin(time * elevationSpeed) * 0.5f;

	LIGHT.dir = {
		cos(elevation) * cos(azimuth),
		sin(elevation),
		cos(elevation) * sin(azimuth),
		0.f
	};
	LIGHT.color = { 1.f,1.f,1.f,0.f };
	
	shader.UpdateConstantBuffer("LightDirectional", &LIGHT, sizeof(LIGHT));







	shader.Bind();
	GetContext()->DrawIndexed(index_count, 0, 0);

}



void Application::AddSubdivision() {
	current_Subdivision++;
	std::cout << current_Subdivision << std::endl;
	UpdateSubdivision();
}
void Application::SubSubdivision() {
	if (current_Subdivision <= 1)
		current_Subdivision = 1;
	else
		current_Subdivision--;

	std::cout << current_Subdivision << std::endl;

	UpdateSubdivision();
}

HRESULT Application::UpdateSubdivision() {
	indexBuffer.Reset();

	std::vector<UINT> indexes = makeIndexes(current_Subdivision);


	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = indexes.size() * sizeof(UINT);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA iinit = {};
	iinit.pSysMem = indexes.data();

	framework::directx::GetDevice()->CreateBuffer(&ibd, &iinit, indexBuffer.GetAddressOf());


	INDEXES.PointsPerRow = current_Subdivision;
	shader.UpdateConstantBuffer("Indexes", &INDEXES, sizeof(INDEXES));
	shader.BindConstantBufferToVS("Indexes");

	index_count = indexes.size();

	return S_OK;
}





void Application::Render() {
	framework::directx::BeginScene(0.f, 0.f, 0.f, 1.f);

	Frame();

	framework::directx::EndScene();
}

void Application::Update() {
	framework::core::Update();

	using namespace framework::input;

	if (IsKeyPressed(KEY_N))
		SubSubdivision();
	else if (IsKeyPressed(KEY_M))
		AddSubdivision();

	if (IsMousePressed(MouseButton_LEFT)) {
		framework::window::ToggleClipCursor();
	}

	camera.Update();

}