#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>
#include <sstream>
#include <iostream>

#include "adjustable.h"

class Camera : public cv::VideoCapture, public Adjustable {
	public:
		Camera(bool adjustable,unsigned int camId);
		
		static const unsigned int width=1280;
		static const unsigned int height=960;
		static const unsigned int framerate=30;
		
		static constexpr double focal=0.0039;
		static constexpr double pixelSize=0.0000028;
		
	private:
		int v4l(std::string args);
		int updateV4l(std::string name);
		int updateV4lAll();

		unsigned int _camId;
	
		virtual void adjusted(std::string name,int val);
};

#endif

