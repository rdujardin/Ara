#ifndef LOG_WINDOW_H
#define LOG_WINDOW_H

#include <opencv2/opencv.hpp>
#include <cmath>
#include <sstream>

#include "common.h"
#include "adjustable.h"
#include "timer.h"

class LogWindow : public std::ostream {
	public:
		LogWindow();
		
	private:
		cv::Mat* _logMat;
};

#endif

