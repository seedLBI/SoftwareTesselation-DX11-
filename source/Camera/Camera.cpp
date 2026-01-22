#include "Camera.h"

#include <iostream>

Camera::Camera() {
	Pos.SetAll({ 0.f,0.f,-2.4f });
	Front.SetAll({ 0.f,0.f,-1.f });
	Up.SetAll({ 0.f,1.f,0.f });

	yaw.SetAll(-90.f);
	pitch.SetAll(0.f);
	fov.SetAll(45.f);

	roll.SetAll(0.f); 
	delta_yaw = 0.f;
}

Camera::~Camera() {

}


XMMATRIX Camera::getMatrixView() {
	XMMATRIX view = XMMatrixLookAtRH(
		Pos.GetLerped(),
		XMVectorAdd(Pos.GetLerped(), Front.GetLerped()),
		Up.GetLerped()
	);

	return view;
}


float& Camera::GetFOV() {
	return fov.GetLerped();
}
XMVECTOR& Camera::GetPos() {
	return Pos.GetLerped();
}
XMVECTOR& Camera::GetFront() {
	return Front.GetLerped();
}
XMVECTOR& Camera::GetUp() {
	return Up.GetLerped();
}
XMVECTOR Camera::GetRight() {
	XMVECTOR result = XMVector3Normalize(
		XMVector3Cross(
			Front.GetLerped(), 
			Up.GetLerped()
		)
	);

	return result;
}


void Camera::UpdateMovement() {
	float cameraSpeed = 0.8f * framework::time::GetDeltaTime();

	using namespace framework::input;

	if (IsKeyDown(KEY_LEFT_SHIFT))
		cameraSpeed *= 5.f;

	if (IsKeyDown(KEY_W))
		Pos.GetReal() -= cameraSpeed * Front.GetReal();

	if (IsKeyDown(KEY_S))
		Pos.GetReal() += cameraSpeed * Front.GetReal();

	if (IsKeyDown(KEY_A))
		Pos.GetReal() -= cameraSpeed * XMVector3Normalize(
			XMVector3Cross(
				Front.GetReal(), 
				Up.GetReal()
			)
		);

	if (IsKeyDown(KEY_D))
		Pos.GetReal() += cameraSpeed * XMVector3Normalize(
			XMVector3Cross(
				Front.GetReal(), 
				Up.GetReal()
			)
		);

	if (IsKeyDown(KEY_SPACE))
		Pos.GetReal() += cameraSpeed * XMVector3Normalize(
			Up.GetReal()
		);

	if (IsKeyDown(KEY_LEFT_CONTROL))
		Pos.GetReal() -= cameraSpeed * XMVector3Normalize(
			Up.GetReal()
		);


}
void Camera::UpdateLook() {
	auto delta = framework::input::GetDelta();

	float sensitivity = -0.02f;
	float xoffset = static_cast<float>(delta.x) * sensitivity;
	float yoffset = static_cast<float>(delta.y) * sensitivity * -1.f;

	yaw.GetReal()   += xoffset;
	pitch.GetReal() += yoffset;

	if (pitch.GetReal() > 89.0f)
		pitch.GetReal() = 89.0f;
	if (pitch.GetReal() < -89.0f)
		pitch.GetReal() = -89.0f;
	delta_yaw = xoffset;


	float r_yaw   = XMConvertToRadians(yaw.GetReal());
	float r_pitch = XMConvertToRadians(pitch.GetReal());

	XMFLOAT3 front;
	front.x = cos(r_yaw) * cos(r_pitch);
	front.y = sin(r_pitch);
	front.z = sin(r_yaw) * cos(r_pitch);

	XMVECTOR front_vec = XMLoadFloat3(&front);

	Front.Set(XMVector3Normalize(front_vec));

}
void Camera::UpdateFOV() {

	float wheelValue = 2.f * (float)framework::input::GetWheelDelta();

	if (fov.GetReal() >= 1.0f && fov.GetReal() <= 90.0f)
		fov.GetReal() -= wheelValue;
	if (fov.GetReal() <= 1.0f)
		fov.GetReal() = 1.0f;
	if (fov.GetReal() >= 90.0f)
		fov.GetReal() = 90.0f;
}


void Camera::UpdateRoll() {
	using namespace framework::input;
	float strafe_dir = 0.f;
	if (IsKeyDown(KEY_A)) strafe_dir -= 1.f;
	if (IsKeyDown(KEY_D)) strafe_dir += 1.f;

	float strafe_roll_max = 5.f;
	float strafe_roll = -strafe_dir * strafe_roll_max;

	float dt = framework::time::GetDeltaTime();
	if (dt == 0.f) dt = 0.016f;
	float yaw_rate = delta_yaw / dt;

	float turn_roll_factor = 0.025f;
	float turn_roll = yaw_rate * turn_roll_factor;

	float target_roll = strafe_roll + turn_roll;
	roll.GetReal() = target_roll;
}


void Camera::UpdateUp() {
	XMVECTOR world_up = XMVectorSet(0.f, 1.f, 0.f,0.f);
	XMVECTOR front = Front.GetLerped();
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(front, world_up));
	XMVECTOR up_level = XMVector3Normalize(XMVector3Cross(right, front));

	float phi = XMConvertToRadians(roll.GetLerped());
	XMVECTOR new_up = XMVectorAdd(
		XMVectorScale(up_level, cos(phi)),
		XMVectorScale(right, sin(phi))
	);
	new_up = XMVector3Normalize(new_up);

	Up.Set(new_up);
}



void Camera::UpdateLerp() {

	float speed = 12.f;
	float time = 1.f - std::exp(-speed * framework::time::GetDeltaTime());

	Pos.Update(time);
	Front.Update(time);
	Up.Update(time);
	yaw.Update(time);
	pitch.Update(time);
	fov.Update(time);
	roll.Update(time);


	XMFLOAT3 pos, front, up;

	XMStoreFloat3(&pos, Pos.GetLerped());
}


void Camera::Update() {

	UpdateMovement();
	UpdateLook();
	UpdateFOV();
	UpdateRoll();
	UpdateLerp();
	UpdateUp();

}