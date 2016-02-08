#ifndef VISION_H
#define VISION_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <sstream>
#include <ctime>

#include "timer.h"
#include "hsv.h"
#include "filters.h"
#include "hough.h"
#include "camera.h"
#include "detectors.h"

enum State {
	PLACE_BALL=0,
	RUNNING=1
};

struct Position {
	int x,y,z;
};

class BallDetector {
	public:
		BallDetector(bool withBallPlacing,bool withGeneralSettings,bool withCamSettings,bool withGui,bool withBenchmarking);
		~BallDetector();
		
		bool loop(Position& pos);
		
		static constexpr double ballRadius=0.065;
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
		
		std::map<std::string,Timable*> _timables;
};

#endif

