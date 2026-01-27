#include "Application.h"

#include <iostream>

Application::Application(HINSTANCE hInstance, int nCmdShow) {
	framework::core::Init(hInstance, nCmdShow);

	framework::time::SetFPS(0);
	framework::directx::EnableVSync();



	std::cout << "[SHADER] - Trying compile shaders\n";
	shader.LoadShadersFromFile(L"resources\\shaders\\shader_pbr.hlsl");
	shader_billboard_light.LoadShadersFromFile_withGS(L"resources\\shaders\\shadel_light_sprite.hlsl");

	std::cout << "[CBUFFER] - Initing cbuffers\n";
	cbuffer_MatrixView	= new ConstantBuffer<c_MatrixView>(0);
	cbuffer_Utils		= new ConstantBuffer<c_Utils>(1);
	cbuffer_Light		= new ConstantBuffer<c_Light>(2);
	cbuffer_ObjectInfo  = new ConstantBuffer<c_ObjectInfo>(3);


	std::cout << "[CBUFFER] - Binding\n";
	cbuffer_MatrixView->BindToAll(0);
	cbuffer_Utils->BindToAll(1);
	cbuffer_Light->BindToAll(2);
	cbuffer_ObjectInfo->BindToAll(3);

	
	std::cout << "[PBR] - Load\n";
	pbrData.LoadFromFolder("resources\\pbr_textures\\tile");
	pbrData2.LoadFromFolder("resources\\pbr_textures\\bricks");

	std::cout << "[PBR] - Bind [bricks]\n";
	pbrData2.BindToAll();
	

	std::cout << "[SAMPLER STATE] - Creting\n";
	Samp = SamplerState::CreateLinearWrap();

	std::cout << "[SAMPLER STATE] - Binding\n";
	Samp.BindToPS(0);
	Samp.BindToVS(0);



	current_Subdivision = 32;


	UpdateSubdivision();


}

Application::~Application() {
	framework::core::Terminate();
}

inline float fract(float x)
{
	return x - std::floor(x);
}

void Application::Frame() {
	using namespace framework::directx;

	GetContext()->IASetInputLayout(nullptr);
	UINT stride = 0;
	UINT offset = 0;
	ID3D11Buffer* nullVB[1] = { nullptr };
	GetContext()->IASetVertexBuffers(0, 0, nullptr, &stride, &offset);



	shader.Bind();

	OBJECT_INFO.ao = 1.f;
	OBJECT_INFO.albedo = { 0.5f,0.f,0.f };

	const int sphereRows = 8;
	const int sphereColumns = 8;
	const float spacing = 2.5;
	for (size_t y = 0; y <= sphereRows; y++){

		float metallic = (float)y / (float)sphereRows;
		OBJECT_INFO.metallic = metallic;


		for (size_t x = 0; x <= sphereColumns; x++) {

			float roughness = (float)x / (float)sphereColumns;
			if (roughness < 0.05f)
				roughness = 0.05f;


			OBJECT_INFO.roughness = roughness;

			float mid_col = float(sphereColumns) / 2.f;
			float mid_row = float(sphereRows) / 2.f;
			float col = (float)x;
			float row = (float)y;
			OBJECT_INFO.pos = { 
				-(col - mid_col) * spacing,
				(row - mid_row) * spacing,
				0.f };




			cbuffer_ObjectInfo->Update(OBJECT_INFO);


			GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			GetContext()->DrawInstanced(6, count_quads, 0, 0);


		}
	}







	// Light render
	shader_billboard_light.Bind();
	GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	GetContext()->DrawInstanced(1, UTILS.count_lights, 0, 0);

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
	std::cout << "[SUBDIVISION] - Update\n";

	count_quads = (current_Subdivision);
	count_quads *= count_quads;

	UTILS.quads_row = current_Subdivision;

	cbuffer_Utils->Update(UTILS);

	return S_OK;
}

void Application::Render() {
	framework::directx::BeginScene(0.f, 0.f, 0.f, 1.f);

	Frame();

	framework::directx::EndScene();
}


void Application::Update_cMATRIX() {
	auto size = framework::window::GetSize();
	float aspect = float(size.x) / float(size.y);
	float r_fov = XMConvertToRadians(camera.GetFOV());

	XMMATRIX proj = XMMatrixPerspectiveFovLH(r_fov, aspect, 0.01f, 100.f);
	XMMATRIX view = camera.getMatrixView();

	XMMATRIX VP = view * proj;

	XMStoreFloat4x4(&MATRIX.WorldViewProj, XMMatrixTranspose(VP));
	XMStoreFloat3(&MATRIX.PosView, camera.GetPos());
	XMStoreFloat3(&MATRIX.FrontView, camera.GetFront());
	XMStoreFloat3(&MATRIX.UpView, camera.GetUp());
	XMStoreFloat3(&MATRIX.RightView, camera.GetRight());
}

void Application::Update_cTIME() {
	UTILS.time = framework::time::GetTimeProgram();

}

void Application::Update_cLIGHT() {

	UTILS.count_lights = 4;

	LIGHT.lights[0].pos = { -10.0f,  10.0f, 10.0f, 0.f };
	LIGHT.lights[1].pos = { 10.0f,  10.0f, 10.0f, 0.f };
	LIGHT.lights[2].pos = { -10.0f, -10.0f, 10.0f, 0.f };
	LIGHT.lights[3].pos = { 10.0f, -10.0f, 10.0f, 0.f };



	LIGHT.lights[0].color = { 300.0f, 300.0f, 300.0f, 0.f };
	LIGHT.lights[1].color = { 300.0f, 300.0f, 300.0f, 0.f };
	LIGHT.lights[2].color = { 300.0f, 300.0f, 300.0f, 0.f };
	LIGHT.lights[3].color = { 300.0f, 300.0f, 300.0f, 0.f };

	
	XMFLOAT4 Camera_pos;
	XMStoreFloat4(&Camera_pos, camera.GetPos());

	auto getLen_to_camera = [&Camera_pos](const XMFLOAT4& light_pos) -> float {

		float dx = Camera_pos.x - light_pos.x;
		float dy = Camera_pos.y - light_pos.y;
		float dz = Camera_pos.z - light_pos.z;

		float len = dx * dx + dy * dy + dz * dz;

		return len;
		};

	for (size_t i = 0; i < UTILS.count_lights; i++) {

		for (size_t j = 0; j < UTILS.count_lights - 1; j++) {

			float len_j1 = getLen_to_camera(LIGHT.lights[j].pos);
			float len_j2 = getLen_to_camera(LIGHT.lights[j + 1].pos);

			if (len_j1 < len_j2)
				std::swap(LIGHT.lights[j], LIGHT.lights[j + 1]);

		}

	}
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

	if (IsKeyPressed(KEY_V))
	{
		pbrData.BindToAll();
	}
	else if (IsKeyPressed(KEY_B)) {
		pbrData2.BindToAll();
	}


	camera.Update();


	Update_cMATRIX();
	Update_cTIME();
	Update_cLIGHT();

	cbuffer_MatrixView->Update(MATRIX);
	cbuffer_Utils->Update(UTILS);
	cbuffer_Light->Update(LIGHT);

}