#ifndef APPLICATION_H
#define APPLICATION_H

#include <DirectXMath.h>
#include "Framework/DX11.Framework.h"
#include "Shader/DX11.Shader.h"
#include <wrl/client.h>

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

	struct cbuffer_MatrixView { 
		XMFLOAT4X4 wvp;
		XMFLOAT3 posView;
	} MVP;

	struct cbuffer_Indexes {
		UINT QuadsPerRow; 
		UINT pad[3];
	} INDEXES;

	struct cbuffer_Time {
		float time;
	} TIME;

	struct cbuffer_LightDirectional {
		XMFLOAT4 dir;
		XMFLOAT4 color;
	} LIGHT;


	size_t current_Subdivision = 2;

	void AddSubdivision();
	void SubSubdivision();

	HRESULT UpdateSubdivision();

	UINT count_quads = 0;

	DX11_Shader shader;


	Camera camera;

};





#endif