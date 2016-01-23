#include "filters.h"

using namespace std;
using namespace cv;

//------------------------------------------------------------------------------

GaussianFilter::GaussianFilter(Timer& timer) : Timable(timer) {}

void GaussianFilter::apply(Mat& img) {
	timerStart();
	GaussianBlur(img, img, Size(3, 3), 2);
	timerStop();
}

void GaussianFilter::operator()(Mat& img) {
	apply(img);
}

//------------------------------------------------------------------------------

DilateEroder::DilateEroder(Timer& timer, bool adjustable) : Timable(timer) {
	
	_dilateSize=5;
	_erodeSize=5;
	
	if(adjustable) {
		createTrackbar("Dilate size","Settings",&_dilateSize,255);
		createTrackbar("Erode size","Settings",&_erodeSize,255);
	}
}

DilateEroder::~DilateEroder() {
	//.
}

void DilateEroder::apply(Mat& img) {
	timerStart();
	
	_dilateStructure=new Mat(_dilateSize,_dilateSize,CV_8UC1);
	_erodeStructure=new Mat(_erodeSize,_erodeSize,CV_8UC1);
	
	circle(*_dilateStructure,Point(_dilateSize/2,_dilateSize/2),_dilateSize/2,255,-1);
	circle(*_erodeStructure,Point(_erodeSize/2,_erodeSize/2),_erodeSize/2,255,-1);
	
	/*erode(img, img, Mat::ones(Size(_erodeSize, _erodeSize), CV_8UC1));
	dilate(img, img, Mat::ones(Size(_dilateSize, _dilateSize), CV_8UC1));*/
	
	erode(img,img,*_erodeStructure);
	dilate(img,img,*_dilateStructure);
	
	delete _dilateStructure;
	delete _erodeStructure;
	
	timerStop();
}

void DilateEroder::operator()(Mat& img) {
	apply(img);
}

//------------------------------------------------------------------------------

