/*
filters.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "filters.h"

using namespace std;
using namespace cv;

//------------------------------------------------------------------------------

GaussianFilter::GaussianFilter(Timer& timer, bool adjustable) : Timable(timer), Adjustable() {
	
	_params["Gaussian width"]=3;//8 //4
	_params["Gaussian height"]=3;//8 //4
	_params["Gaussian sigX"]=5;//10 //3
	
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
	
	_params["Dilate size"]=4;//8 //2
	_params["Erode size"]=4;//8 //2
	_params["Dilate iterations"]=1; //2 //7
	_params["Erode iterations"]=1; //5 //7
	
	if(adjustable) {
		makeAdjustable("Dilate size",40);
		makeAdjustable("Erode size",40);
		makeAdjustable("Dilate iterations",40);
		makeAdjustable("Erode iterations",40);
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
	
	erode(img,img,_erodeStructure,Point(-1,-1),_params["Erode iterations"]);
	dilate(img,img,_dilateStructure,Point(-1,-1),_params["Dilate iterations"]);
	
	timerStop();
}

void DilateEroder::operator()(Mat& img) {
	apply(img);
}

void DilateEroder::adjusted(string name,int val) {
	_params[name]=val;
}

//------------------------------------------------------------------------------

