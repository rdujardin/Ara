#ifndef FILTERS_H
#define FILTERS_H

#include <opencv2/opencv.hpp>
#include "timer.h"

//------------------------------------------------------------------------------

class GaussianFilter : public Timable {
public:
	GaussianFilter(Timer& timer);

	void apply(cv::Mat& img);
	void operator()(cv::Mat& img);
private:

};

//------------------------------------------------------------------------------

class DilateEroder : public Timable {
public:
	DilateEroder(Timer& timer, bool adjustable);
	~DilateEroder();

	void apply(cv::Mat& img);
	void operator()(cv::Mat& img);
	
	int _dilateSize, _erodeSize;
private:
	cv::Mat* _dilateStructure;
	cv::Mat* _erodeStructure;
};

//------------------------------------------------------------------------------

#endif

