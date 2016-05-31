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
#include "logWindow.h"

enum Mode {
	FOLLOW,
	GATHER,
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

struct BotState {
	double alpha1,alpha2,alpha3,theta0,theta3;
	double terminalX,terminalY,terminalZ;
	double wristX,wristY,terminalXTh,length3Al;
};

void copyState(BotState& dst,BotState& src);

#define WORK_W 640
#define WORK_H 480

static double _terminalAbsAlpha=1*M_PI/180;
static double _terminalAbsTheta=0.01*M_PI/180;
static double _terminalYOffset=0;
static double _length1=30;
static double _length2=30;
static double _length3=20;
static double _length3Th=_length3*cos(_terminalAbsAlpha);

typedef std::vector<BotState> Trajectory;
typedef std::vector<BotState>::iterator TrajIt;

double conv(unsigned int servo,bool unit,double input);

#endif

