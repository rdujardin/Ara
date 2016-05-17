/*
logWindow.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "logWindow.h"

using namespace std;
using namespace cv;

Log::Log() {
	color=Scalar(255,255,255);
	size=1;
}

Log::Log(Scalar _color,int _size) {
	color=_color;
	size=_size;
}

Log& Log::operator=(Log const& l) {
	color=l.color;
	size=l.size;
	_stream << l._stream.str();
}

ostringstream& Log::reset() {
	_stream.str(string());
	_stream.clear();
	return _stream;
}

ostringstream& Log::append() {
	return _stream;
}

string Log::read() {
	return _stream.str();
}

LogWindow::LogWindow() {
	_period=0;
	_toRepaint=false;
}

map<string,Log>& LogWindow::logs() {
	return _logs;
}

Log& LogWindow::operator[](std::string id) {
	map<string,Log>::iterator res=_logs.find(id);
	if(res!=_logs.end()) return res->second;
	else {
		_logs[id]=Log();
		return _logs[id];
	}
}

Log& LogWindow::operator[](const char* id) {
	return operator[](string(id));
}

void LogWindow::refresh() {
	_toRepaint=true;
}

void LogWindow::processRepainting() {
	if(_toRepaint && Timer::periodically(50,_period)) {
		Mat logMat=imread("logbg.png");
		unsigned int h=20;
		for(map<string,Log>::iterator it=_logs.begin();it!=_logs.end();++it) {
			putText(logMat,it->second.read(),Point(5,h),1,it->second.size,it->second.color,1);
			h+=20*it->second.size;
		}
		imshow("Logs",logMat);
		_toRepaint=false;
	}
}

