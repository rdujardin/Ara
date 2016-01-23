#include "hsv.h"

using namespace std;
using namespace cv;

HSV_Thresholder::HSV_Thresholder(Timer& timer, bool adjustable) : Timable(timer) {

	/*_h_min = 20;
	_h_max = 54;
	_s_min = 28;
	_s_max = 93;
	_v_min = 102;
	_v_max = 247;*/
	
	_h_min = 26;
	_h_max = 84;
	_s_min = 45;
	_s_max = 96;
	_v_min = 41;
	_v_max = 255;
	
	if(adjustable) {
		createTrackbar("H min","Settings",&_h_min,255);
		createTrackbar("H max","Settings",&_h_max,255);
		createTrackbar("S min","Settings",&_s_min,255);
		createTrackbar("S max","Settings",&_s_max,255);
		createTrackbar("V min","Settings",&_v_min,255);
		createTrackbar("V max","Settings",&_v_max,255);
	}
}

void HSV_Thresholder::apply(Mat& src, Mat& dst) {
	timerStart();
	cvtColor(src, dst, CV_BGR2HSV);
	inRange(dst, Scalar(_h_min, _s_min, _v_min), Scalar(_h_max, _s_max, _v_max), dst);
	timerStop();
}

void HSV_Thresholder::operator()(Mat& src, Mat& dst) {
	apply(src, dst);
}
