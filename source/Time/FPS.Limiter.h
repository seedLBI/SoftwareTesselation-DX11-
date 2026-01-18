#ifndef FPS_LIMITER_H
#define FPS_LIMITER_H

#pragma comment(lib, "winmm.lib")
#include <windows.h>

#include "Timer.h"

class FPSLimiter {
public:
	FPSLimiter();
	~FPSLimiter();

	void Disable();
	void Enable(const int& limit = 60);

	void SetFPS(const float& fps);
	void SetDeltaTime(const float& deltaTime);

	void Process();

	float GetDeltaTime();
	float GetFPS();

private:
	double Delta = 0.0;

	Timer deltaTimer;

	LARGE_INTEGER frequency;
	LARGE_INTEGER lastTime;

	double frameTime = 0.0;
	double targetFPS = 120.0;
};






#endif