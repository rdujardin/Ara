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
#include "logWindow.h"

enum State {
	BD_PLACE_BALL=1,
	BD_RUNNING=2
};

static cv::Mat _kalman=cv::Mat::zeros(WORK_H,WORK_W,CV_8UC3);
static cv::KalmanFilter KF(6,3,0); //KF(nbr of state->(x,y,z,vx,vy,vz),nbr of measured states(x,y,z),number of action control)
//
static cv::Mat_<float> measurement(3,1); 
static std::vector<cv::Point3_<float>> positionv, kalmanv;

class BallDetector {
	public:
		BallDetector(Camera* cam);
		~BallDetector();

		bool loop(Position& detection);
		
		static constexpr double ballRadius=0.072; //tennis : 0.065 ; balle verte : 7 cm + 2 mm de scratch
	private:
		State _state;
		Camera* _cam;
		Timer* _timer;
		HSV_Thresholder* _hsvThresholder;
		GaussianFilter* _gaussianFilter;
		DilateEroder* _dilateEroder;
		Hough* _hough;
		EllipseFitter* _ellipseFitter;
		MomentsCalculator* _momentsDetector;

		cv::Mat _calibMap1, _calibMap2;
		int pourcent;
		cv::Point centre;
		
		std::map<std::string,Timable*> _timables;

		void initKalmanFilter();
		cv::Mat kalmanFilter(double posx,double posy,double posz);
};

#endif

