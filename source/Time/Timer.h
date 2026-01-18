#ifndef TIMER_H
#define TIMER_H

#include <chrono>


class Timer {
public:
	Timer();
	~Timer();

	void start();
	void stop();

	long long elapsedNanoseconds() const;
	long long elapsedMicroseconds() const;
	long long elapsedMilliseconds() const;
	double elapsedSeconds() const;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
	std::chrono::time_point<std::chrono::high_resolution_clock> stop_time;
};


#endif TIMER_H