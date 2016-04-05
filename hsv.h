#ifndef HSV_H
#define HSV_H

#include <opencv2/opencv.hpp>

#include "timer.h"
#include "camera.h"

class HSV_Thresholder : public Timable, public Adjustable {
public:
	HSV_Thresholder(Timer& timer, bool adjustable);

	void apply(cv::Mat& src, cv::Mat& dst);
	void operator()(cv::Mat& src, cv::Mat& dst);
	
	void autoSet(cv::Mat& img);
	
	static const unsigned int autoSetRadius=100; // si changé, changer ballDetector.cpp:118 et hsv.cpp:49
	
private:	
	virtual void adjusted(std::string name,int val);
	
	void createUi();
	
};

#endif

