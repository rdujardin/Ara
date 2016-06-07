/*
botState.h (part of Ara, https://github.com/rdujardin/Ara)

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

#ifndef BOT_STATE_H
#define BOT_STATE_H

#include <vector>
#include <cmath>
#include <map>
#include <sstream>

#include "../common/common.h"

#define angles 0
#define cartesian 1

#define alpha1 1
#define alpha2 2
#define alpha3 3
#define theta0 4
#define theta3 5
#define terminalX 6
#define terminalY 7
#define terminalZ 8
#define wristX 9
#define wristY 10
#define terminalXTh 11
#define length3Al 12
class BotState {
public:
	BotState();
	static BotState readyPos();
	static BotState offPos();
	//void setFrom(BotState& s);
	void setFrom(BotState s);
	double get(unsigned int i);
	bool set(unsigned int type,unsigned int i1,double v1,unsigned int i2=0,double v2=0,unsigned i3=0,double v3=0,unsigned i4=0,double v4=0,unsigned i5=0,double v5=0,unsigned i6=0,double v6=0,unsigned i7=0,double v7=0,unsigned i8=0,double v8=0,unsigned i9=0,double v9=0,unsigned i10=0,double v10=0,unsigned i11=0,double v11=0);
	void setUnsafe(unsigned int type,unsigned int i1,double v1,unsigned int i2=0,double v2=0,unsigned i3=0,double v3=0,unsigned i4=0,double v4=0,unsigned i5=0,double v5=0,unsigned i6=0,double v6=0,unsigned i7=0,double v7=0,unsigned i8=0,double v8=0,unsigned i9=0,double v9=0,unsigned i10=0,double v10=0,unsigned i11=0,double v11=0);
	bool checkWorkingZone();
	std::string stringify();
private:
	void computeAngles();
	void computeCartesian();
	bool check(unsigned int i);
	std::vector<double> _vals;
};

static double _terminalAbsAlpha=/*1*/-45/*-30*/*M_PI/180;
static double _terminalAbsTheta=0.01*M_PI/180;
static double _terminalYOffset=0;
static double _length1=29.5;
static double _length2=30;
static double _length3=27+11;
static double _length3Th=_length3*cos(_terminalAbsAlpha);

typedef std::vector<BotState> Trajectory;
typedef std::vector<BotState>::iterator TrajIt;

double conv(unsigned int servo,bool unit,double input);

#endif
