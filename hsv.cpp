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

void HSV_Thresholder::autoSet(Mat& img,string ball) {

	map<string,unsigned int[6]> hsvBases;
	#define hsvBase(name,a,b,c,d,e,f) hsvBases[name][0]=a; hsvBases[name][1]=b; hsvBases[name][2]=c; hsvBases[name][3]=d; hsvBases[name][4]=e; hsvBases[name][5]=f;
	hsvBase("green",43,78,147,255,39,255);
	hsvBase("tennis",22,70,94,217,66,255);
	
	Mat hsv(img,Rect(Camera::width/2-HSV_AUTOSET_RADIUS/sqrt(2),Camera::height/2-HSV_AUTOSET_RADIUS/sqrt(2),2*HSV_AUTOSET_RADIUS/sqrt(2),2*HSV_AUTOSET_RADIUS/sqrt(2)));
	cvtColor(hsv,hsv,CV_BGR2HSV);
	
	vector<Mat> channels;
	split(hsv,channels);
	
	double min,max;

	minMaxIdx(channels[0],&min,&max);
	min-=HSV_RANGE_EXTEND;
	if(min<0) min=0;
	max+=HSV_RANGE_EXTEND;
	if(max>255) max=255;
	_params["H min"]=hsvBases[ball][0]; //37 min
	_params["H max"]=hsvBases[ball][1]; //84 max

	minMaxIdx(channels[1],&min,&max);
	min-=HSV_RANGE_EXTEND/**10*/;
	if(min<0) min=0;
	max+=HSV_RANGE_EXTEND/**10*/;
	if(max>255) max=255;
	max=255;
	_params["S min"]=hsvBases[ball][2]; //52min
	_params["S max"]=hsvBases[ball][3]; //max
	
	minMaxIdx(channels[2],&min,&max);
	min-=HSV_RANGE_EXTEND/**7*/;
	if(min<0) min=0;
	max+=HSV_RANGE_EXTEND/**7*/;
	if(max>255) max=255;
	_params["V min"]=hsvBases[ball][4]; //96min
	_params["V max"]=hsvBases[ball][5]; //max
	
	createUi();
	
}


