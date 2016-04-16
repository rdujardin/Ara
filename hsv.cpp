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
	
	//Mat hsv(img,Rect(Camera::width/2-autoSetRadius/**4/sqrt(2)*/,Camera::height/2-autoSetRadius/**4/sqrt(2)*/,autoSetRadius*2/*8/sqrt(2)*/,autoSetRadius*2/*8/sqrt(2)*/)); //*4,*4,*8,*8
	//Mat hsv(img,Rect(320,240,640,480));
	//Mat hsv(img,Rect(Camera::width/2-100,Camera::height/2-100,200,200));
	Mat hsv(img,Rect(Camera::width/2-HSV_AUTOSET_RADIUS/sqrt(2),Camera::height/2-HSV_AUTOSET_RADIUS/sqrt(2),2*HSV_AUTOSET_RADIUS/sqrt(2),2*HSV_AUTOSET_RADIUS/sqrt(2)));
	imshow("Debug",hsv);
	cvtColor(hsv,hsv,CV_BGR2HSV);
	
	vector<Mat> channels;
	split(hsv,channels);
	
	double min,max;

	minMaxIdx(channels[0],&min,&max);
	min-=HSV_RANGE_EXTEND;
	if(min<0) min=0;
	max+=HSV_RANGE_EXTEND;
	if(max>255) max=255;
	_params["H min"]=43; //37 min
	_params["H max"]=78; //84 max

	minMaxIdx(channels[1],&min,&max);
	min-=HSV_RANGE_EXTEND/**10*/;
	if(min<0) min=0;
	max+=HSV_RANGE_EXTEND/**10*/;
	if(max>255) max=255;
	max=255;
	_params["S min"]=147; //52min
	_params["S max"]=255; //max
	
	minMaxIdx(channels[2],&min,&max);
	min-=HSV_RANGE_EXTEND/**7*/;
	if(min<0) min=0;
	max+=HSV_RANGE_EXTEND/**7*/;
	if(max>255) max=255;
	_params["V min"]=39; //96min
	_params["V max"]=255; //max
	
	createUi();
	
}


