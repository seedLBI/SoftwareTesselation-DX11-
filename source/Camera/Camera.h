#ifndef CAMERA_H
#define CAMERA_H


#include "Framework/DX11.Framework.h"
#include <DirectXMath.h>
using namespace DirectX;

#include "LerpedValue/LerpedValue.h"




class Camera
{
public:
	Camera();
	~Camera();

	void Update();

	XMMATRIX getMatrixView();
	float& GetFOV();
	XMVECTOR& GetPos();

private:
	LerpedValue_float3 Pos, Front, Up;
	LerpedValue_float yaw, pitch, roll;
	LerpedValue_float fov;

	float delta_yaw;

	float lastX, lastY;
	bool firstMouse = true;

	void UpdateMovement();
	void UpdateLook();
	void UpdateUp();
	void UpdateRoll();
	void UpdateFOV();
	void UpdateLerp();


};





#endif