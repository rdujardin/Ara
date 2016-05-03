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
	if(Timer::periodically(50,_period)) {
		Mat logMat=imread("logbg.png");
		unsigned int h=20;
		for(map<string,Log>::iterator it=_logs.begin();it!=_logs.end();++it) {
			putText(logMat,it->second.read(),Point(5,h),1,it->second.size,it->second.color,1);
			h+=20*it->second.size;
		}
		imshow("Logs",logMat);
	}
}

