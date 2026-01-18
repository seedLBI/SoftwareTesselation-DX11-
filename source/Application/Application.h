#ifndef APPLICATION_H
#define APPLICATION_H

#include <DirectXMath.h>
#include "Framework/DX11.Framework.h"
#include "Shader/DX11.Shader.h"
#include <wrl/client.h>

#include "SoftwareTesselation/SoftwareTesselation.h"

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
	} MVP;

	struct cbuffer_Indexes {
		UINT PointsPerRow; 
		UINT pad[3];
	} INDEXES;

	struct cbuffer_Time {
		float time;
	} TIME;


	size_t current_Subdivision = 2;

	void AddSubdivision();
	void SubSubdivision();

	HRESULT UpdateSubdivision();

	UINT index_count = 0;

	ComPtr<ID3D11Buffer> indexBuffer;
	DX11_Shader shader;


	Camera camera;

};





#endif