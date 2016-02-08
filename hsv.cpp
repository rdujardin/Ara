#include "hsv.h"

using namespace std;
using namespace cv;

HSV_Thresholder::HSV_Thresholder(Timer& timer, bool adjustable) : Timable(timer), Adjustable() {

	_params["H min"]=24;
	_params["H max"]=36;
	_params["S min"]=185;
	_params["S max"]=255;
	_params["V min"]=141;
	_params["V max"]=255;
	
	if(adjustable) {
		createUi();
	}
}

void HSV_Thresholder::apply(Mat& src, Mat& dst) {
	timerStart();
	cvtColor(src, dst, CV_BGR2HSV);
	inRange(dst, Scalar(_params["H min"],_params["S min"],_params["V min"]), Scalar(_params["H max"], _params["S max"], _params["V max"]), dst);
	timerStop();
}

void HSV_Thresholder::operator()(Mat& src, Mat& dst) {
	apply(src, dst);
}

void HSV_Thresholder::adjusted(string name,int val) {
	_params[name]=val;
}

void HSV_Thresholder::createUi() {
	makeAdjustable("H min",255);
	makeAdjustable("H max",255);
	makeAdjustable("S min",255);
	makeAdjustable("S max",255);
	makeAdjustable("V min",255);
	makeAdjustable("V max",255);
}

void HSV_Thresholder::autoSet(Mat& img) {
	
	Mat hsv(img,Rect(Camera::width/2-autoSetRadius*4/sqrt(2),Camera::height/2-autoSetRadius*4/sqrt(2),autoSetRadius*8/sqrt(2),autoSetRadius*8/sqrt(2)));
	cvtColor(hsv,hsv,CV_BGR2HSV);
	
	vector<Mat> channels;
	split(hsv,channels);
	
	double min,max;

	minMaxIdx(channels[0],&min,&max);
	_params["H min"]=min;
	_params["H max"]=max;

	minMaxIdx(channels[1],&min,&max);
	_params["S min"]=min;
	_params["S max"]=max;
	
	minMaxIdx(channels[2],&min,&max);
	_params["V min"]=min;
	_params["V max"]=max;
	
	createUi();
	
}


