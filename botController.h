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

enum BotControllerState {
	BC_START_UP=1,
	BC_RUNNING=2,
	BC_SHUT_DOWN=3
};

typedef std::vector<double> BotState;

class BotController : public Adjustable {
	public:
		BotController(bool withBot);
		~BotController();

		bool loop(Position detection);

		void startUpRoutine();
		void shutDownRoutine();
		void nextState();
		
		virtual void adjusted(std::string name,int val);
	private:
		bool _withBot;
		BotControllerState _state;

		std::vector<BotState> _routineTrajectory;
		std::vector<BotState>::iterator _rtIt;

		static constexpr double _terminalAbsAlpha=-1*M_PI/180;
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
		
		double _alpha1,_alpha2,_alpha3,_theta0,_theta3;
		double _terminalX,_terminalY,_terminalZ;
		double _wristX,_wristY,_terminalXTh,_length3Al;
		static constexpr double _length3Th=_length3*cos(_terminalAbsAlpha);
		
		void drawAxis(cv::Mat& draw1,cv::Mat& draw2);
		void drawBot(cv::Mat& draw1,cv::Mat& draw2,bool workZoneCheck);

		int _fd;
		bool initSerial();
		void sendInt(int v);
		void sendAngle(double angle);
		void sendToMotors();

		bool loopAngles();

		void receiveVoltage();
		int _batteryLevel;

		double conv(unsigned int servo,bool unit,double input);
		bool checkWorkingZone();
};

#endif

