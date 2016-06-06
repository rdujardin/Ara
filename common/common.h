/*
common.h (part of Ara, https://github.com/rdujardin/Ara)

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

#ifndef COMMON_H
#define COMMON_H

#include <thread>
#include <mutex>
#include <iostream>
#include <queue>
#include <ctime>
#include <cmath>
#include "logWindow.h"

enum Mode {
	NULL_MODE,
	FOLLOW,
	GATHER,
	GATHER_GRAB,
	GATHER_BACK,
	MANUAL,
	STARTUP,
	TRAJECTORY,
	PRESHUTDOWN,
	SHUTDOWN
};

struct Position {
	bool valid;
	double x,y,z;
};
static std::queue<Position> detection;

#define WORK_W 640
#define WORK_H 480

#define neart(x,y,t) (abs(x-y)<=t)
#define near(x,y) (abs(x-y)<=0.01*180/M_PI)

#define ALPHA1_2_RANGE 125

#define nearAng1(a,b,id,t) (abs(a.get(id)-b.get(id))<=t)
#define nearAng(a,b,t) (abs(a.get(theta0)-b.get(theta0))<=t && abs(a.get(alpha1)-b.get(alpha1))<=t && abs(a.get(alpha2)-b.get(alpha2))<=t && abs(a.get(alpha3)-b.get(alpha3))<=t && abs(a.get(theta3)-b.get(theta3))<=t)

#endif

