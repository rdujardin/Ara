/*
logWindow.h (part of Ara, https://github.com/rdujardin/Ara)

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
		void processRepainting();

		Log& operator[](std::string id);
		Log& operator[](const char *id);
		
	private:
		std::map<std::string,Log> _logs;
		double _period;
		bool _toRepaint;
};

static LogWindow logs;

#endif

