/*
detectors.h (part of Ara, https://github.com/rdujardin/Ara)

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

#ifndef DETECTORS_H
#define DETECTORS_H

#include <opencv2/opencv.hpp>

#include "../common/common.h"
#include "../common/timer.h"
#include "../common/adjustable.h"

struct Detection {
	cv::RotatedRect ellipseRect;
	int x,y,radius;
	bool valid;
};

typedef std::vector<Detection> DetectionList;

void drawDetections(cv::Mat& dst,DetectionList& detect,bool ellipses);

//------------------------------------------------------------------------------

class EllipseFitter : public Timable {
public:
	EllipseFitter(Timer& timer);

	void apply(cv::Mat& img,DetectionList& out);
	void apply(cv::Mat& img,cv::Mat& dst,DetectionList& out);
	void operator()(cv::Mat& img,DetectionList& out);
	void operator()(cv::Mat& img,cv::Mat& dst,DetectionList& out);
private:
	
};

//------------------------------------------------------------------------------

class MomentsCalculator : public Timable {
public:
	MomentsCalculator(Timer& timer);

	void apply(cv::Mat& img,cv::Mat& drawOut,DetectionList& out);
	void operator()(cv::Mat& img,cv::Mat& drawOut,DetectionList& out);
};

#endif

