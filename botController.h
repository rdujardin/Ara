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
/*
struct BotState {
	BotState(double _base,double _shou,double _elbo,double _wris,double _wrisTh) : base(_base), shou(_shou), elbo(_elbo), wris(_wris), wrisTh(_wrisTh) {}
	BotState(BotState const& bs) {
		copyFrom(bs);
	}
	BotState& operator=(BotState bs) {
		std::cout << "PROUT!" << std::endl;
		base=bs.base;
		shou=bs.shou;
		elbo=bs.elbo;
		wris=bs.wris;
		wrisTh=bs.wrisTh;
		return *this;
	}
	void copyFrom(BotState bs) {
		base=bs.base;
		shou=bs.shou;
		elbo=bs.elbo;
		wris=bs.wris;
		wrisTh=bs.wrisTh;
	}
	double& num(int id) {
		if(id==0) return base;
		else if(id==1) return shou;
		else if(id==2) return elbo;
		else if(id==3) return wris;
		else if(id==4) return wrisTh;
	}
	double& operator[](int id) { return num(id); }
	double base,shou,elbo,wris,wrisTh;
};
*/
typedef std::vector<double> BotState;

class BotController : public Adjustable {
	public:
		BotController(Mode mode,bool withBot,bool withGui,bool adjustable);
		~BotController();

		bool loop(Position detection);
		
		virtual void adjusted(std::string name,int val);
	private:
		static constexpr double _terminalAbsAlpha=-1*M_PI/180;
		static constexpr double _terminalAbsTheta=0.01*M_PI/180;
		static constexpr double _length1=30;
		static constexpr double _length2=30;
		static constexpr double _length3=20;
		
		static constexpr double _drawScale=6.5;
		static constexpr unsigned int _drawWidth=640;
		static constexpr unsigned int _drawHeight=480;
		static const cv::Point _drawOrigin;
		
		bool _withBot,_withGui,_adjustable;
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
		int safe(int v);
		void sendAngle(double angle);
		void sendToMotors();

		bool loopAngles();
		void startUpRoutine();
		void shutDownRoutine();
		void routine(bool startUp);

		void receiveVoltage();
		unsigned int _batteryLevel;

		Mode _mode;
};

#endif

