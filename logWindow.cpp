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

ostringstream& Log::write() {
	_stream.clear();
	return _stream;
}

ostringstream& Log::read() {
	return _stream;
}

LogWindow::LogWindow() {
	namedWindow("Logs",1);
	_logMat=Mat::zeros(360,360,CV_8UC3);
	imshow("Logs",_logMat);
}

map<string,Log>& LogWindow::logs() {
	return _logs;
}

Log& LogWindow::operator[](std::string id) {
	return _logs[id];
}

void LogWindow::refresh() {
	_logMat=Mat::zeros(360,360,CV_8UC3);
	unsigned int h=10;
	for(map<string,Log>::iterator it=_logs.begin();it!=_logs.end();++it) {
		putText(_logMat,it->second.read().str(),Point(10,h),FONT_HERSHEY_COMPLEX_SMALL,it->second.size,it->second.color,1);
		h+=20*it->second.size;
	}
	imshow("Logs",_logMat);
}


