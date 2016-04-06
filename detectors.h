#ifndef DETECTORS_H
#define DETECTORS_H

#include <opencv2/opencv.hpp>

#include "common.h"
#include "timer.h"
#include "adjustable.h"

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

