/*
botController.h (part of Ara, https://github.com/rdujardin/Ara)

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

#ifndef BOT_CONTROLLER_H
#define BOT_CONTROLLER_H

#include <opencv2/opencv.hpp>
#include <cmath>
#include <sstream>
#include <iostream>

#include "common.h"
#include "adjustable.h"
#include "timer.h"
#include "botState.h"
#include "botHardware.h"
#include "botDraw.h"
#include "botTrajectories.h"

class BotController : public Adjustable {
	public:
		BotController(bool withBot);
		~BotController();

		void setMode(Mode mode);

		bool follow(Position detection);
		bool loopGather(Position detection);
		bool loopManual();
		bool loopRoutine();

		void nextState();
		
		virtual void adjusted(std::string name,int val);
	//private:
		bool _withBot;
		Mode _mode;

		BotHardware* _hardware;

		BotState _state;
		int _vehicleLeftSpeed, _vehicleRightSpeed;

		Trajectory _trajectory;
		TrajIt _trajIt;
		
		BotState computeAngles(BotState state);

		bool loopAngles(bool unsafe=false);
		
};

#endif

