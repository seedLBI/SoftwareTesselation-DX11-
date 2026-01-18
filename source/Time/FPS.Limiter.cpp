#include "FPS.Limiter.h"
#include "timeapi.h"

FPSLimiter::FPSLimiter() {
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTime);
    timeBeginPeriod(1);
	Enable();
}

FPSLimiter::~FPSLimiter() {
	timeEndPeriod(1);
}



void FPSLimiter::Disable() {
	SetFPS(0);
}
void FPSLimiter::Enable(const int& limit) {
	SetFPS((float)limit);
}

void FPSLimiter::SetFPS(const float& fps) {
	targetFPS = fps;
	frameTime = 1.0 / targetFPS;
}
void FPSLimiter::SetDeltaTime(const float& deltaTime) {
	SetFPS(1.f / deltaTime);
}

void FPSLimiter::Process() {
	deltaTimer.stop();
	Delta = deltaTimer.elapsedSeconds();

	if (targetFPS == 0) {
		deltaTimer.start();
		return;
	}

	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	double elapsedTime = static_cast<double>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;



	if (elapsedTime < frameTime) {
		DWORD sleepTime = static_cast<DWORD>((frameTime - elapsedTime) * 1000.0);
		if (sleepTime != 0)
			--sleepTime;
		if (sleepTime > 0)
			Sleep(sleepTime);

		do {
			QueryPerformanceCounter(&currentTime);
			elapsedTime = static_cast<double>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
		} while (elapsedTime < frameTime);
	}

	Delta = elapsedTime;
	lastTime = currentTime;

	deltaTimer.start();
}

float FPSLimiter::GetDeltaTime() {
	return (float)Delta;
}

float FPSLimiter::GetFPS() {
	return 1.f / (float)Delta;
}