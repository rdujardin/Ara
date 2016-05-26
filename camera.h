/*
camera.h (part of Ara, https://github.com/rdujardin/Ara)

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

