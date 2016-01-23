#ifndef HSV_H
#define HSV_H

#include <opencv2/opencv.hpp>
#include "timer.h"

class HSV_Thresholder : public Timable {
public:
	HSV_Thresholder(Timer& timer, bool adjustable);

	void apply(cv::Mat& src, cv::Mat& dst);
	void operator()(cv::Mat& src, cv::Mat& dst);
	
//private:
	/*static const unsigned int*/ int	_h_min ,
		_h_max,
		_s_min ,
		_s_max,
		_v_min ,
		_v_max ;
};

#endif

