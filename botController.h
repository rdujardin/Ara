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

#include <wiringPi.h>
#include <wiringSerial.h>

#include "common.h"
#include "adjustable.h"
#include "timer.h"

struct BotState {
	double alpha1,alpha2,alpha3,theta0,theta3;
	double terminalX,terminalY,terminalZ;
	double wristX,wristY,terminalXTh,length3Al;
};

class BotController : public Adjustable {
	public:
		BotController(bool withBot);
		~BotController();

		void setMode(Mode mode);

		bool follow(Position detection);
		bool loopGather(Position detection);
		bool loopManual();

		bool loopStartUpRoutine();
		bool loopShutDownRoutine();

		void nextState();
		
		virtual void adjusted(std::string name,int val);
	private:
		bool _withBot;
		Mode _mode;

		BotState _state;
		int _vehicleLeftSpeed, _vehicleRightSpeed;

		void initStartUpRoutine();
		void initShutDownRoutine();

		void manual();

		//std::vector<BotState> _routineTrajectory;
		//std::vector<BotState>::iterator _rtIt;

		std::vector<BotState> _trajectory;
		std::vector<BotState>::iterator _trajIt;

		static constexpr double _terminalAbsAlpha=1*M_PI/180;
		static constexpr double _terminalAbsTheta=0.01*M_PI/180;
		static constexpr double _length1=30;
		static constexpr double _length2=30;
		static constexpr double _length3=20;
		
		static constexpr double _drawScale=6.5;
		static constexpr unsigned int _drawWidth=640;
		static constexpr unsigned int _drawHeight=480;
		static const cv::Point _drawOrigin;
		
		std::string _inputX,_inputY,_inputZ;
		static const std::string _strCliQuit;
		
		static constexpr double _length3Th=_length3*cos(_terminalAbsAlpha);
		
		BotState computeAngles(BotState state);

		void drawAxis(cv::Mat& draw1,cv::Mat& draw2);
		void drawBot(cv::Mat& draw1,cv::Mat& draw2,bool workZoneCheck);

		int _fd;
		bool initSerial();
		void sendInt(int v);
		void sendAngle(double angle);
		void sendToMotors();
		void sendToVehicle();

		bool loopAngles();

		void checkBatteryLevel();
		int _batteryLevel;

		double conv(unsigned int servo,bool unit,double input);
		bool checkWorkingZone();

		void genTrajectory(Position a,Position b);
};

#endif

