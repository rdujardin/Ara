/*
ballDetector.h (part of Ara, https://github.com/rdujardin/Ara)

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
		BallDetector(Camera* cam,std::string ball);
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

