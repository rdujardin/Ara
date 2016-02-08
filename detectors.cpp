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
	draw(dst,out);
}

void EllipseFitter::operator()(Mat& img,DetectionList& out) {
	apply(img,out);
}

void EllipseFitter::operator()(Mat& img,Mat& dst,DetectionList& out) {
	apply(img,dst,out);
}

void EllipseFitter::draw(Mat& dst,DetectionList& detect) {
	unsigned int color=0;
	for(DetectionList::const_iterator it=detect.begin();it!=detect.end();++it) {
		Point center(round(it->x),round(it->y));
		circle(dst,center,3,Scalar(0,255,0),2);
		circle(dst,center,it->radius,Scalar(color,0,255-color),3);
		if(color<=250) color+=50;
		
		Point2f rectVertices[4];
		it->ellipseRect.points(rectVertices);
		for(int i=0;i<4;i++) {
			line(dst,rectVertices[i],rectVertices[(i+1)%4],Scalar(0,255,0));
		}
	}
}

//------------------------------------------------------------------------------

