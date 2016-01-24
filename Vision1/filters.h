#ifndef FILTERS_H
#define FILTERS_H

#include <opencv2/opencv.hpp>
#include "timer.h"
#include "adjustable.h"

//------------------------------------------------------------------------------

class GaussianFilter : public Timable, public Adjustable {
public:
	GaussianFilter(Timer& timer, bool adjustable);

	void apply(cv::Mat& img);
	void operator()(cv::Mat& img);
private:
	virtual void adjusted(std::string name,int val);
};

//------------------------------------------------------------------------------

class DilateEroder : public Timable, public Adjustable {
public:
	DilateEroder(Timer& timer, bool adjustable);
	~DilateEroder();

	void apply(cv::Mat& img);
	void operator()(cv::Mat& img);
private:
	//cv::Mat* _dilateStructure;
	//cv::Mat* _erodeStructure;
	
	virtual void adjusted(std::string name,int val);
};

//------------------------------------------------------------------------------

#endif

