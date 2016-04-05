#include "ballDetector.h"

using namespace std;
using namespace cv;

BallDetector::BallDetector(bool withBallPlacing,bool withGeneralSettings,bool withCamSettings,bool withGui,bool withBenchmarking) {
	_withGui=withGui;
	_withBenchmarking=withBenchmarking;
	_state=(withBallPlacing)?PLACE_BALL:RUNNING;
	
	if(_withGui) {
		cv::namedWindow("Input", 1);
		cv::namedWindow("Output", 1);
		cv::namedWindow("Hsv", 1);
		cv::namedWindow("Debug",1);
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

	//KALMAN
	pourcent=0;
	centre=Point(2*320,2*240);
	//Initialisation of the filter
	KF.transitionMatrix=(Mat_<float>(4,4) << 1,0,1,0, 0,1,0,1, 0,0,0.6,0, 0,0,0,0.6); //Sans * au début de la parenthèse
	measurement.setTo(Scalar(0));

	KF.statePost.at<float>(0)=0;
	KF.statePost.at<float>(1)=0;
	KF.statePost.at<float>(2)=0;
	KF.statePost.at<float>(3)=0;
	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov,Scalar::all(1e-2)); //Rapidité du "rattrapage" de l'estimation
	setIdentity(KF.measurementNoiseCov,Scalar::all(10));
	setIdentity(KF.errorCovPost,Scalar::all(1));
	//---	

	//CALIBRAGE
	Mat distCoeffs=(Mat_<double>(5,1) << -4.4806416419013684e-01,4.1928815312056306e-01,0,0,-4.8777148182526148e-01);
	Mat cameraMatrix=(Mat_<double>(3,3) << 6.4106316770762089e+02,0,3.195e+02,0,6.4106316770762089e+02,2.395e+02,0,0,1);
	Mat map1,map2;

	Size imageSize=Size(640,480);
	initUndistortRectifyMap(cameraMatrix,distCoeffs,Mat(),
	getOptimalNewCameraMatrix(cameraMatrix,distCoeffs,imageSize,1,imageSize,0),
	imageSize,CV_16SC2,map1,map2);
	_map1=map1;
	_map2=map2;
	ofstream fichier("test.txt");
	fichier << _map1;
	//---

	_cam=new Camera(true);
	if (!_cam->isOpened()) throw -1;
	
	_timer=new Timer();
	_hsvThresholder=new HSV_Thresholder(*_timer, true);
	_gaussianFilter=new GaussianFilter(*_timer, true);
	_dilateEroder=new DilateEroder(*_timer, true);
	_ellipseFitter=new EllipseFitter(*_timer);
	_momentsDetector=new MomentsCalculator(*_timer);
	
	_timables["Hsv"]=_hsvThresholder;
	_timables["Blur"]=_gaussianFilter;
	_timables["DilEr"]=_dilateEroder;
	_timables["Ellipse"]=_ellipseFitter;
	_timables["Moments"]=_momentsDetector;
}

BallDetector::~BallDetector() {
	delete _momentsDetector;
	delete _ellipseFitter;
	delete _dilateEroder;
	delete _gaussianFilter;
	delete _hsvThresholder;
	delete _timer;
	delete _cam;
}

bool BallDetector::loop(Position& detection) {

	_timer->reset();

	Mat input(Camera::width,Camera::height,CV_8UC3);
	Mat output(Camera::width,Camera::height,CV_8UC3);

	bool newFrame=_cam->read(input);
	if(_state!=PLACE_BALL && !newFrame) {
		if(waitKey(30)>=0) return false;
		else return true;
	}

	Mat resized(Camera::width,Camera::height,CV_8UC3);
	//resize(input,resized,Size(320,240));
	//resized=input;
	input.copyTo(resized);

	//remap(input,input,_map1,_map2,INTER_LINEAR); //?                 //  <<<<<<<<  REMAP HERE <<<<<<< 
	vector<Point> poisition_filterless(1); //?
	vector<Point> position_filtered(1); //?

	if(_state==PLACE_BALL) {
		circle(resized,Point(Camera::width/2,Camera::height/2),/*HSV_Thresholder::autoSetRadius*/100,Scalar(0,0,255),4);
		Mat flipped(Camera::width,Camera::height,CV_8UC3);
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
	
		DetectionList detections;
		//_ellipseFitter->apply(output,resized,detections);
		_momentsDetector->apply(output,resized,detections);
		
		double detX=0,detY=0,detR=-1;
		for(DetectionList::const_iterator it=detections.begin();it!=detections.end();++it) {
			detX+=it->x;
			detY+=it->y;
			if(it->radius>detR) detR=it->radius;
		}
		detX/=detections.size();
		detY/=detections.size();

		Position pos;
		
		if(!detections.empty()) {
			double z=_cam->focal*ballRadius/(2*((double) detR)*4*_cam->pixelSize);
			detection.z=z*100;
			cout << "u : " << detections[0].x << " / v : " << detections[0].y << " / r : " << detections[0].radius << " ### ";
			//detections[0]
			Mat filtered_position=kalmanFilter(detX,detY); //?
			detection.x=100*((((double) filtered_position.at<float>(0))*4-640)*_cam->pixelSize*z/_cam->focal); //detX
			detection.y=100*((480-((double) filtered_position.at<float>(1))*4)*_cam->pixelSize*z/_cam->focal); //detY
		}

		if(_withGui) {
			imshow("Input", resized);
			imshow("Output", output);
			imshow("2D Position",_kalman); //?
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

//KALMAN
cv::Mat BallDetector::kalmanFilter(double posx,double posy) {
	//mem.copyTo(im);
	//First predict, to update the internal statePre variable
	Mat prediction=KF.predict();
	Point predictPt(prediction.at<float>(0),prediction.at<float>(1));
	//Get ball position
	measurement(0)=2*posx;
	measurement(1)=2*posy;
	//The update phase
	Mat estimated=KF.correct(measurement);

	Point statePt(estimated.at<float>(0),estimated.at<float>(1));
	Point measPt(measurement(0),measurement(1));
	//plot points
	//activer si dessin sur fond noir

	positionv.push_back(measPt);
	kalmanv.push_back(statePt);

	if(positionv.size()>50) {
		positionv.erase(positionv.begin());
		kalmanv.erase(kalmanv.begin());
		_kalman=Scalar::all(0);
	}
	drawCross(statePt,Scalar(255,255,255),5);
	drawCross(measPt,Scalar(0,0,255),5);

	for(int i=0;i<positionv.size()-1;i++) {
		line(_kalman,positionv[i],positionv[i+1],Scalar(255,255,0),1);
		cout << "brut = " << positionv[i];
	}

	for(int i=0;i<kalmanv.size();i++) {
		line(_kalman,kalmanv[i],kalmanv[i+1],Scalar(0,0,255),1);
		cout << "_kalman = " << kalmanv[i] << endl;
	}

	if(posx>2*320+40 || posx<2*320-40) {
		//circle(_kalman,centre,2*abs(320-posx),Scalar(0,0,255),1,1,1);
		pourcent=2*abs(320-posx)*100/(_kalman.cols); //2*Eloignement / largeur_image %
		cout << "Puissance moteur = " << pourcent << "%" << endl;
		//putText(im,"TEST",centre,5,10,0,Scalar(0,0,255));
	}

	cout << "x brut = " << posx;
	cout << " | y brut = " << posy << endl;
	cout << "x _kalman = " << estimated.at<float>(0);
	cout << " | y _kalman = " << estimated.at<float>(1) << endl;

	return estimated;
}

