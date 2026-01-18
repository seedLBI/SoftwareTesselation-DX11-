#ifndef TIMER_HIGH_RESOLUTION_H
#define TIMER_HIGH_RESOLUTION_H

#include <windows.h>

class TimerHighResolution {
public:
	TimerHighResolution();
	~TimerHighResolution();

	void begin();
	void end();

	float getTimeElapsed();


private:
	LARGE_INTEGER freq;
	LARGE_INTEGER beginTime;
	LARGE_INTEGER endTime;
};


#endif