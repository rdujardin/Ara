#ifndef TIMER_H
#define TIMER_H

#include <ctime>
#include <string>
#include <map>

class Timer;

class Timable {
public:
	Timable(Timer& timer);
	double time();
protected:
	void timerStart();
	void timerStop();
private:
	Timer& _timer;
};

class Timer {
public:
	Timer();

	void reset();

	void start(Timable* obj);
	void end(Timable* obj);

	double get(Timable* obj);
	double total();

	static void wait(unsigned int ms);
private:
	std::map<Timable*, clock_t> _timers;
	clock_t _total;

	static double toMilliseconds(clock_t timer);
};

#endif

