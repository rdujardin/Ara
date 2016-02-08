#include "ballDetector.h"

using namespace std;
using namespace cv;

BallDetector::BallDetector(bool withBallPlacing,bool withGeneralSettings,bool withCamSettings,bool withGui,bool withBenchmarking) {
	_withGui=withGui;
	_withBenchmarking=withBenchmarking;
	_state=(withBallPlacing)?PLACE_BALL:RUNNING;
	
	if(_withGui) {
		cv::namedWindow("Input", 1);
		cv::namedWindow("Output", 2);
		cv::namedWindow("Hsv", 4);
		if(withGeneralSettings) {
			moveWindow("Input",520,40);
			moveWindow("Hsv",520,308);
			moveWindow("Output",520,576);
		}
		else {
			moveWindow("Input",10,10);
			moveWindow("Hsv",350,10);
			moveWindow("Output",700,10);
		}
	}

	if(withGeneralSettings) {
		cv::namedWindow("Settings", 3);
		moveWindow("Settings",200,40);
	}
	if(withCamSettings) {
		cv::namedWindow("Camera Settings", 5);
		moveWindow("Camera Settings",840,40);
	}
	
	_cam=new Camera(true);
	if (!_cam->isOpened()) throw -1;
	
	_timer=new Timer();
	_hsvThresholder=new HSV_Thresholder(*_timer, true);
	_gaussianFilter=new GaussianFilter(*_timer, true);
	_dilateEroder=new DilateEroder(*_timer, true);
	_ellipseFitter=new EllipseFitter(*_timer);
	
	_timables["Hsv"]=_hsvThresholder;
	_timables["Blur"]=_gaussianFilter;
	_timables["DilEr"]=_dilateEroder;
	_timables["Ellipse"]=_ellipseFitter;
}

BallDetector::~BallDetector() {
	delete _ellipseFitter;
	delete _dilateEroder;
	delete _gaussianFilter;
	delete _hsvThresholder;
	delete _timer;
	delete _cam;
}

bool BallDetector::loop(Position& pos) {
	
	_timer->reset();

	Mat input, output;
	(*_cam) >> input;

	Mat resized;
	resize(input,resized,Size(320,240));

	if(_state==PLACE_BALL) {
		circle(resized,Point(160,120),HSV_Thresholder::autoSetRadius,Scalar(0,0,255),4);
		Mat flipped;
		flip(resized,flipped,1);
	
		if(_withGui) {
			imshow("Input",flipped);
			imshow("Hsv",flipped);
			imshow("Output",flipped);
		}
	
		if(waitKey(30)>=0) {
			_hsvThresholder->autoSet(input);
			_state=RUNNING;
		}
	}

	else {

		_gaussianFilter->apply(resized);

		_hsvThresholder->apply(resized, output);

		if(_withGui) imshow("Hsv", output);

		_dilateEroder->apply(output);
	
		DetectionList detections;;
		_ellipseFitter->apply(output,resized,detections);
		
		double detX=0,detY=0,detR=-1;
		for(DetectionList::const_iterator it=detections.begin();it!=detections.end();++it) {
			detX+=it->x;
			detY+=it->y;
			if(it->radius>detR) detR=it->radius;
		}
		detX/=detections.size();
		detY/=detections.size();
		
		if(!detections.empty()) {
			double z=_cam->focal*ballRadius/(2*((double) detR)*4*_cam->pixelSize);
			pos.x=100*((((double) detX)*4-640)*_cam->pixelSize*z/_cam->focal);
			pos.y=100*((480-((double) detY)*4)*_cam->pixelSize*z/_cam->focal);
			pos.z=z*100;
			cout << "u : " << detections[0].x << " / v : " << detections[0].y << " / r : " << detections[0].radius << " ### ";
		}

		if(_withGui) {
			imshow("Input", resized);
			imshow("Output", output);
		}
		
		if(_withBenchmarking) {
			cout << "BENCHMARK ## " << "Total : " << _timer->total() << " ms | ";
			for(map<string,Timable*>::const_iterator it=_timables.begin();it!=_timables.end();++it) {
				cout << it->first << " : " << it->second->time() << " ms , ";
			}
			cout << endl;
		}

		if (waitKey(30) >= 0) return false;
	}
	return true;
}

