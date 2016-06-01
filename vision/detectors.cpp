/*
detectors.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "detectors.h"

using namespace std;
using namespace cv;

//------------------------------------------------------------------------------

EllipseFitter::EllipseFitter(Timer& timer) : Timable(timer){}

void EllipseFitter::apply(Mat& img,DetectionList& out) {
	timerStart();
	
	vector< vector<Point> > contours;
	Mat tmp=img.clone();
	findContours(tmp,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	for(vector< vector<Point> >::const_iterator it=contours.begin();it!=contours.end();++it) {
		Point2f center;
		float radius;
		Detection d;
		
		if(it->size()>=5) {
			RotatedRect ellipseRect=fitEllipse(*it);
			d.ellipseRect=ellipseRect;
			d.x=(int) ellipseRect.center.x;
			d.y=(int) ellipseRect.center.y;
			d.radius=(int) min(ellipseRect.size.width,ellipseRect.size.height)/2;
			if(d.radius>0) out.push_back(d);
		}
	}
	
	timerStop();
}

void EllipseFitter::apply(Mat& img,Mat& dst,DetectionList& out) {
	apply(img,out);
	//drawDetections(dst,out,true);
}

void EllipseFitter::operator()(Mat& img,DetectionList& out) {
	apply(img,out);
}

void EllipseFitter::operator()(Mat& img,Mat& dst,DetectionList& out) {
	apply(img,dst,out);
}

void drawDetections(Mat& dst,DetectionList& detect,bool ellipses) {
	unsigned int color=0;
	for(DetectionList::const_iterator it=detect.begin();it!=detect.end();++it) {
		Point center(round(it->x),round(it->y));
		circle(dst,center,3,Scalar(0,255,0),2);
		circle(dst,center,it->radius,Scalar(color,0,255-color),3);
		if(color<=250) color+=50;
		
		if(ellipses) {
			Point2f rectVertices[4];
			it->ellipseRect.points(rectVertices);
			for(int i=0;i<4;i++) {
				line(dst,rectVertices[i],rectVertices[(i+1)%4],Scalar(0,255,0));
			}
		}
	}
}

//------------------------------------------------------------------------------

MomentsCalculator::MomentsCalculator(Timer& timer) : Timable(timer){}

void MomentsCalculator::apply(Mat& img,Mat& drawOut,DetectionList& out) {
	timerStart();

	Moments m=moments(img,true);

	Detection d;
	d.x=m.m10/m.m00;
	d.y=m.m01/m.m00;
	d.radius=m.m00/*30*/;
//	d.ellipseRect=
	if(d.x<0 || d.x>WORK_W || d.y<0 || d.y>WORK_H) d.valid=false;
	else d.valid=true;
	if(d.valid) out.push_back(d);

	timerStop();

	//drawDetections(drawOut,out);
}

void MomentsCalculator::operator()(Mat& img,Mat& drawOut,DetectionList& out) {
	apply(img,drawOut,out);
}

