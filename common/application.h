/*
application.h (part of Ara, https://github.com/rdujardin/Ara)

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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <cstring>
#include <iostream>

#include "common.h"
#include "../vision/ballDetector.h"
#include "../bot/botController.h"
#include "logWindow.h"

#define KEY_ESCAPE 1048603 //27
#define KEY_SPACE 1048608// 32
#define KEY_BACKSPACE 1113864 // 8
#define KEY_ENTER 1048586 //13

#define EVENT_CONTINUE 0
#define EVENT_EXIT 1

class Application {
	public:
		Application(int argc,char* argv[]);
		~Application();

	private:
		void setMode(Mode mode,Mode next=NULL_MODE);

		int checkKeyboard();

		Mode _mode,_nextMode,_optMode;
		bool _pause;
		bool _gatherGo;

		Camera* _cam;
		BallDetector* _ballDetector;
		BotController* _botController;

		bool _optWithBot;
		bool _optForceWithRoutines;
		unsigned int _optCamId;
		std::string _optBall;
		double _optCoordCalib;

		void readArgs(int argc,char* argv[]);
		void adaptPosition(Position& pos);
		void adaptOrientation(Position& pos);
		void initWindows();
};

#endif

