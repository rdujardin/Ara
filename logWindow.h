#ifndef LOG_WINDOW_H
#define LOG_WINDOW_H

#include <opencv2/opencv.hpp>
#include <cmath>
#include <sstream>

#include "adjustable.h"
#include "timer.h"

class Log {
	public:
		Log();
		Log(cv::Scalar _color,int _size);

		Log& operator=(Log const& l);
		
		std::ostringstream& reset();
		std::ostringstream& append();
		std::string read();
	
		cv::Scalar color;
		int size;
	private:
		std::ostringstream _stream;
};

class LogWindow {
	public:
		LogWindow();

		std::map<std::string,Log>& logs();
		void refresh();

		Log& operator[](std::string id);
		Log& operator[](const char *id);
		
	private:
		cv::Mat _logMat;
		std::map<std::string,Log> _logs;
};

#endif

