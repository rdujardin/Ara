#ifndef BOT_CONTROLLER_H
#define BOT_CONTROLLER_H

#include <opencv2/opencv.hpp>
#include <cmath>
#include <sstream>

#include <wiringPi.h>
#include <wiringSerial.h>

#include "common.h"
#include "adjustable.h"
#include "timer.h"

struct BotState {
	BotState(double _base,double _shou,double _elbo,double _wris,double _wrisTh) : base(_base), shou(_shou), elbo(_elbo), wris(_wris), wrisTh(_wrisTh) {}
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

class BotController : public Adjustable {
	public:
		BotController(bool withBot,bool withGui,bool adjustable);
		~BotController();

		bool loop(Position detection);
		
		virtual void adjusted(std::string name,int val);
	private:
		static constexpr double _terminalAbsAlpha=20*M_PI/180;
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
		void drawBot(cv::Mat& draw1,cv::Mat& draw2);

		int _fd;
		bool initSerial();
		void sendInt(int v);
		int safe(int v);
		void sendAngle(double angle);
		void sendToMotors();

		bool loopAngles();
		void startUpRoutine();
		void shutDownRoutine();
};

#endif

