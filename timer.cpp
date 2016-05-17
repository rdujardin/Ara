/*
timer.cpp (part of Ara, https://github.com/rdujardin/Ara)

Copyright (c) 2016, Raphaël Dujardin (rdujardin) & Jean Boehm (jboehm1)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

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
	return Timer::toMilliseconds(_timers[obj]);
}

double Timer::total() {
	return Timer::toMilliseconds(_total);
}

double Timer::toMilliseconds(clock_t timer) {
	return ((double)timer) / CLOCKS_PER_SEC * 1000;
}

void Timer::wait(unsigned int ms) {
	double t=Timer::toMilliseconds(clock());
	while(Timer::toMilliseconds(clock())-t<ms);
}

bool Timer::periodically(unsigned int ms,double& time) {
	double now=Timer::toMilliseconds(clock());
	if(now-time>=ms) {
		time=now;
		return true;
	}
	else return false;
}

