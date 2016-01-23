#include "timer.h"

using namespace std;

Timable::Timable(Timer& timer) : _timer(timer) {}

double Timable::time() {
	return _timer.get(this);
}

void Timable::timerStart() {
	_timer.start(this);
}

void Timable::timerStop() {
	_timer.end(this);
}

Timer::Timer() {
	reset();
}

void Timer::reset() {
	_total = 0;
	_timers.clear();
}

void Timer::start(Timable* obj) {
	_timers[obj] = clock();
}

void Timer::end(Timable* obj) {
	/*map<Timable&,clock_t>::iterator iobj = _timers.find(obj);
	if (iobj == _timers.end()) {
		throw "Timable not found !";
		return;
	}*/
	_timers[obj] = clock() - _timers[obj];
	_total += _timers[obj];
}

double Timer::get(Timable* obj) {
	/*map<Timable&, clock_t>::iterator iobj = _timers.find(obj);
	if (iobj == _timers.end()) {
		throw "Timable not found !";
		return -1;
	}*/
	return toMilliseconds(_timers[obj]);
}

double Timer::total() {
	return toMilliseconds(_total);
}

double Timer::toMilliseconds(clock_t timer) {
	return ((double)timer) / CLOCKS_PER_SEC * 1000;
}

