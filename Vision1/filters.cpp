#include "filters.h"

using namespace std;
using namespace cv;

//------------------------------------------------------------------------------

GaussianFilter::GaussianFilter(Timer& timer, bool adjustable) : Timable(timer), Adjustable() {
	
	_params["Gaussian width"]=4;//8
	_params["Gaussian height"]=4;//8
	_params["Gaussian sigX"]=3;//10
	
	if(adjustable) {
		makeAdjustable("Gaussian width",30);
		makeAdjustable("Gaussian height",30);
		makeAdjustable("Gaussian sigX",30);
	}
	
}

void GaussianFilter::apply(Mat& img) {
	timerStart();
	GaussianBlur(img, img, Size(_params["Gaussian width"]*2+1, _params["Gaussian height"]*2+1), _params["Gaussian sigX"]);
	timerStop();
}

void GaussianFilter::operator()(Mat& img) {
	apply(img);
}

void GaussianFilter::adjusted(string name,int val) {
	_params[name]=val;
}

//------------------------------------------------------------------------------

DilateEroder::DilateEroder(Timer& timer, bool adjustable) : Timable(timer), Adjustable() {
	
	_params["Dilate size"]=2;//8
	_params["Erode size"]=2;//8
	
	if(adjustable) {
		makeAdjustable("Dilate size",40);
		makeAdjustable("Erode size",40);
	}
}

DilateEroder::~DilateEroder(){
	//.
}

void DilateEroder::apply(Mat& img) {
	timerStart();
	
	int _dilateSize=_params["Dilate size"];
	int _erodeSize=_params["Erode size"];
	
	Mat _dilateStructure=Mat::zeros(_dilateSize,_dilateSize,CV_8UC1);
	Mat _erodeStructure=Mat::zeros(_erodeSize,_erodeSize,CV_8UC1);
	
	circle(_dilateStructure,Point(_dilateSize/2,_dilateSize/2),_dilateSize/2,255,-1);
	circle(_erodeStructure,Point(_erodeSize/2,_erodeSize/2),_erodeSize/2,255,-1);
	
	erode(img,img,_erodeStructure);
	dilate(img,img,_dilateStructure);
	
	timerStop();
}

void DilateEroder::operator()(Mat& img) {
	apply(img);
}

void DilateEroder::adjusted(string name,int val) {
	_params[name]=val;
}

//------------------------------------------------------------------------------

