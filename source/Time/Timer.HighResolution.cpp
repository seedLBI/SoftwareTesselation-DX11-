#include "Timer.HighResolution.h"


TimerHighResolution::~TimerHighResolution() {

}

TimerHighResolution::TimerHighResolution() : freq(), beginTime(), endTime() {
	QueryPerformanceFrequency(&freq);
}

void TimerHighResolution::begin() {
	QueryPerformanceCounter(&beginTime);
}

void TimerHighResolution::end() {
	QueryPerformanceCounter(&endTime);
}

float TimerHighResolution::getTimeElapsed() {
	float elapsedTime = float( double(endTime.QuadPart - beginTime.QuadPart) / (double)freq.QuadPart);
	return elapsedTime;
}