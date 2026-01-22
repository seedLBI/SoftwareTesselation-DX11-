#ifndef APPLICATION_H
#define APPLICATION_H

#include <DirectXMath.h>
#include "Framework/DX11.Framework.h"

#include <wrl/client.h>

#include "PBR_TextureObject/PBR.TextureObject.h"

#include "DX11/Shader/DX11.Shader.h"
#include "DX11/SamplerState/SamplerState.h"
#include "DX11/ConstantBuffer/ConstantBuffer.h"

#include "Camera/Camera.h"



using namespace DirectX;

class Application {
public:
	Application(HINSTANCE hInstance, int nCmdShow);
	~Application();

	void Render();
	void Update();

private:
	void Frame();

	PBR_TextureObject pbrData;
	PBR_TextureObject pbrData2;
	SamplerState Samp;

	struct c_MatrixView { 
		XMFLOAT4X4 WorldViewProj;
		XMFLOAT3 PosView;
		float padding_pos;
		XMFLOAT3 FrontView;
		float padding_front;
		XMFLOAT3 UpView;
		float padding_up;
		XMFLOAT3 RightView;
		float padding_right;
	} MATRIX;

	struct c_Utils {
		float time;
		UINT  quads_row;
		UINT  count_lights;
		float padding;
	} UTILS;


	struct LightObject {
		XMFLOAT4 pos;
		XMFLOAT4 color;
	};

	struct c_Light {
		LightObject lights[16];
	} LIGHT;
	
	ConstantBuffer<c_MatrixView>*   cbuffer_MatrixView = 0;
	ConstantBuffer<c_Utils>*		cbuffer_Utils = 0;
	ConstantBuffer<c_Light>*		cbuffer_Light = 0;


	void Update_cTIME();
	void Update_cMATRIX();
	void Update_cLIGHT();


	size_t current_Subdivision = 2;

	void AddSubdivision();
	void SubSubdivision();

	HRESULT UpdateSubdivision();

	UINT count_quads = 0;

	DX11_Shader shader;
	DX11_Shader shader_billboard_light;


	Camera camera;

};





#endif