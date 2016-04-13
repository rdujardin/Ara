#ifndef VISION_H
#define VISION_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <sstream>
#include <ctime>
#include <vector>
#include <fstream>

#include "common.h"
#include "timer.h"
#include "hsv.h"
#include "filters.h"
#include "hough.h"
#include "camera.h"
#include "detectors.h"

#define drawCross(center,color,d)

enum State {
	PLACE_BALL=0,
	RUNNING=1
};

static cv::Mat _kalman=cv::Mat::zeros(WORK_H,WORK_W,CV_8UC3);
static cv::KalmanFilter KF(4,2,0);
static cv::Mat_<float> measurement(2,1);
static std::vector<cv::Point> positionv, kalmanv;

class BallDetector {
	public:
		BallDetector(Mode mode,bool withBot,bool withBallPlacing,bool withGeneralSettings,bool withCamSettings,bool withGui,bool withBenchmarking);
		~BallDetector();

		bool loop(Position& detection);
		
		static constexpr double ballRadius=0.072; //tennis : 0.065 ; balle verte : 7 cm + 2 mm de scratch
	private:
		bool _withGui,_withBenchmarking;
		State _state;
		Camera* _cam;
		Timer* _timer;
		HSV_Thresholder* _hsvThresholder;
		GaussianFilter* _gaussianFilter;
		DilateEroder* _dilateEroder;
		Hough* _hough;
		EllipseFitter* _ellipseFitter;
		MomentsCalculator* _momentsDetector;

		cv::Mat _map1, _map2;
		int pourcent;
		cv::Point centre;
		
		std::map<std::string,Timable*> _timables;

		cv::Mat kalmanFilter(double posx,double posy);

		Mode _mode;
		bool _withBot;
};

#endif

