#ifndef DETECTORS_H
#define DETECTORS_H

#include <opencv2/opencv.hpp>

#include "timer.h"
#include "adjustable.h"

struct Detection {
	cv::RotatedRect ellipseRect;
	int x,y,radius;
};

typedef std::vector<Detection> DetectionList;

//------------------------------------------------------------------------------

class EllipseFitter : public Timable {
public:
	EllipseFitter(Timer& timer);

	void apply(cv::Mat& img,DetectionList& out);
	void apply(cv::Mat& img,cv::Mat& dst,DetectionList& out);
	void operator()(cv::Mat& img,DetectionList& out);
	void operator()(cv::Mat& img,cv::Mat& dst,DetectionList& out);
private:
	void draw(cv::Mat& dst,DetectionList& detect);
};

//------------------------------------------------------------------------------

#endif

