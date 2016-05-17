#include "ballDetector.h"

using namespace std;
using namespace cv;

BallDetector::BallDetector(Camera* cam) {
	_state=BD_RUNNING;

	initKalmanFilter();

	//Calibrage caméra
	Mat distCoeffs=(Mat_<double>(5,1) << -4.4806416419013684e-01,4.1928815312056306e-01,0,0,-4.8777148182526148e-01);
	Mat cameraMatrix=(Mat_<double>(3,3) << 6.4106316770762089e+02,0,3.195e+02,0,6.4106316770762089e+02,2.395e+02,0,0,1);
	Size imageSize=Size(640,480);
	initUndistortRectifyMap(cameraMatrix,distCoeffs,Mat(),getOptimalNewCameraMatrix(cameraMatrix,distCoeffs,imageSize,1,imageSize,0),imageSize,CV_16SC2,_calibMap1,_calibMap2);
	//---

	_cam=cam;
	
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

	Mat vachier=Mat::zeros(Camera::width,Camera::height,CV_8UC3);
	_hsvThresholder->autoSet(vachier);
}

BallDetector::~BallDetector() {
	delete _momentsDetector;
	delete _ellipseFitter;
	delete _dilateEroder;
	delete _gaussianFilter;
	delete _hsvThresholder;
	delete _timer;
}

bool BallDetector::loop(Position& detection) {

	_timer->reset();

	Mat input(Camera::width,Camera::height,CV_8UC3);
	Mat output(Camera::width,Camera::height,CV_8UC3);

	bool newFrame=_cam->read(input);

	Mat resized(WORK_W,WORK_H,CV_8UC3);
	resize(input,resized,Size(WORK_W,WORK_H));

	remap(resized,resized,_calibMap1,_calibMap2,INTER_LINEAR);

	if(_state==BD_PLACE_BALL) {
		circle(resized,Point(WORK_W/2,WORK_H/2),HSV_AUTOSET_RADIUS,Scalar(0,0,255),4);
		Mat flipped(WORK_W,WORK_H,CV_8UC3);
		flip(resized,flipped,1);
	
		imshow("Input",flipped);
		imshow("Output",flipped);
	}

	else {
		Mat out;
		resized.copyTo(out);
		_gaussianFilter->apply(resized);

		_hsvThresholder->apply(resized, output);

		_dilateEroder->apply(output);
	
		DetectionList detections;
		_ellipseFitter->apply(output,out,detections); //switch ellipse/moments : switcher le true/false line 186 (true pour ellipses)
		//_momentsDetector->apply(output,resized,detections);
		
		/*double detX=0,detY=0,detR=-1,detValid=false;
		if(!detections.empty()) {
			for(DetectionList::const_iterator it=detections.begin();it!=detections.end();++it) {
				if(it->valid) {
					detX+=it->x;
					detY+=it->y;
					if(it->radius>detR) detR=it->radius;
				}			
			}
			if(detX<0 || detX>WORK_W || detY<0 || detY>WORK_H || detR<=0) detValid=false;
			else detValid=true;
		}

		if(detValid) {
			double z=_cam->focal*ballRadius/(2*((double) sqrt(detR)/2*1.5)*4*_cam->pixelSize); //*1.5 : empirique (on considère qu'on a en général les deux tiers de la balle)
			detection.z=z*100;
			Mat filtered_position=kalmanFilter(detX,detY);
			detection.x=100*((((double) filtered_position.at<float>(0))*2-WORK_W)*_cam->pixelSize*z/_cam->focal);
			detection.y=100*((WORK_H-((double) filtered_position.at<float>(1))*2)*_cam->pixelSize*z/_cam->focal);
			detection.valid=true;
			ostringstream oss;
			oss << " (" << detection.x << "," << detection.y << "," << sqrt(detR)/2 << "," << string((detection.valid)?"VALID":"NOPE") << ")";
			putText(resized,oss.str(),Point(10,10),1,1,Scalar(255,0,0),1);

			DetectionList outDetections;
			Detection outD;
			outD.x=detX;
			outD.y=detY;
			outD.radius=sqrt(detR)/2*1.5; //*1.5 : empirique (on considère qu'on a en général les deux tiers de la balle)
			outD.valid=true;
			outDetections.push_back(outD);
			drawDetections(resized,outDetections,true);
		}
		else detection.valid=false;*/

		//OLD ELLIPSES CODE BEGIN
		drawDetections(out,detections,true);
		double detX=0,detY=0,detR=-1;
		/*for(DetectionList::const_iterator it=detections.begin();it!=detections.end();++it) {
			detX+=it->x;
			detY+=it->y;
			if(it->radius>detR) detR=it->radius;
		}
		detX/=detections.size();
		detY/=detections.size();*/
		double maxR=0;
		DetectionList::const_iterator maxRIt=detections.begin();
		for(DetectionList::const_iterator it=detections.begin();it!=detections.end();++it) {
			if(it->radius>maxR) {
				maxR=it->radius;
				maxRIt=it;
			}
		}
		detX=maxRIt->x;
		detY=maxRIt->y;
		detR=maxR;

		if(!detections.empty()) {
			double z=_cam->focal*ballRadius/(2*((double) detR)*4*_cam->pixelSize);
			//detection.z=z*100;
			//cout << "u : " << detections[0].x << " / v : " << detections[0].y << " / r : " << detections[0].radius << " ### ";
			//detections[0]
			Mat filtered_position=kalmanFilter(detX,detY,z); //?
			detection.x=100*((((double) filtered_position.at<float>(0))*2-WORK_W)*_cam->pixelSize*z/_cam->focal); //detX
			detection.y=100*((WORK_H-((double) filtered_position.at<float>(1))*4)*_cam->pixelSize*z/_cam->focal); //detY
			
			detection.z=100*(double) filtered_position.at<float>(2);			

			if(detection.x<0 || detection.x>WORK_W || detection.y<0 || detection.y>WORK_H) detection.valid=false;
			else detection.valid=true;
		}
		else detection.valid=false;
		//OLD ELLIPSES CODE END

		imshow("Input", out);
		imshow("Output", output);
		imshow("Trajectory",_kalman);
		
		logs["Benchmark"].reset() << "BENCHMARK ## " << "Total : " << _timer->total() << " ms | ";
		/*for(map<string,Timable*>::const_iterator it=_timables.begin();it!=_timables.end();++it) {
			logs["Benchmark"].append() << it->first << " : " << it->second->time() << " ms , ";
		}*/
		logs.refresh();

		
	}
	return true;
}

void BallDetector::initKalmanFilter() {
	pourcent=0;
	centre=Point(WORK_W/2,WORK_H/2);

/*2D*/	//KF.transitionMatrix=(Mat_<float>(4,4) << 1,0,1,0, 0,1,0,1, 0,0,0.6,0, 0,0,0,0.6); //Sans * au début de la parenthèse

/*3D*/	KF.transitionMatrix=(Mat_<float>(6,6) << 1,0,0,1,0,0, 0,1,0,0,1,0, 0,0,1,0,0,1, 0,0,0,0.6,0,0, 0,0,0,0,0.6,0, 0,0,0,0,0,0.6);

/*	 x y dx dy
	| 1 0 0 0 |
	| 0 1 0 0 |
	| 0 0 1 0 |
	| 0 0 0 1 |
Cas 3D:
         x y z dx dy dz
	| 1 0 0 1 0 0 |
	| 0 1 0 0 1 0 |
	| 0 0 1 0 0 1 |	
	| 0 0 0 1 0 0 |	
	| 0 0 0 0 1 0 |
	| 0 0 0 0 0 1 |*/

	measurement.setTo(Scalar(0));

	KF.statePost.at<float>(0)=0;//initial x position
	KF.statePost.at<float>(1)=0;//initial y position
	KF.statePost.at<float>(2)=0;//initial x speed
	KF.statePost.at<float>(3)=0;//initial y speed
	KF.statePost.at<float>(4)=0;
	KF.statePost.at<float>(5)=0;
	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov,Scalar::all(0.05)); //1e-2 Rapidité du "rattrapage" de l'estimation
	setIdentity(KF.measurementNoiseCov,Scalar::all(10));
	setIdentity(KF.errorCovPost,Scalar::all(1));
}

cv::Mat BallDetector::kalmanFilter(double posx,double posy, double posz) {
	
	//First predict, to update the internal statePre variable
	Mat prediction=KF.predict();
	Point3_<float> predictPt(prediction.at<float>(0),prediction.at<float>(1),prediction.at<float>(2));
	//Get ball position
	measurement(0)=posx;
	measurement(1)=posy;
	measurement(2)=posz;
	//The update phase
	Mat estimated=KF.correct(measurement);

	Point3_<float> statePt(estimated.at<float>(0),estimated.at<float>(1),estimated.at<float>(2));
	Point3_<float> measPt(measurement(0),measurement(1),measurement(2));
	 
	//plot points
	//activer si dessin sur fond noir

	positionv.push_back(measPt);
	kalmanv.push_back(statePt);

	if(positionv.size()>50) {
		positionv.erase(positionv.begin());
		kalmanv.erase(kalmanv.begin());
		_kalman=Scalar::all(0);
	}

	for(int i=0;i<positionv.size()-1;i++) {
		line(_kalman,Point(positionv[i].x,positionv[i].y),Point(positionv[i+1].x,positionv[i+1].y),Scalar(255,255,0),1);
	}

	for(int i=0;i<kalmanv.size()-1;i++) {
		if(positionv.size()>=50) line(_kalman,Point(kalmanv[i].x,kalmanv[i].y),Point(kalmanv[i+1].x,kalmanv[i+1].y),Scalar(0,0,255),1);
	}

	if(posx>WORK_W/2+40 || posx<WORK_H/2-40) {
		//circle(_kalman,centre,2*abs(320-posx),Scalar(0,0,255),1,1,1);
		pourcent=abs(WORK_W/2-posx)*100/(_kalman.cols); //2*Eloignement / largeur_image %
		//cout << "Puissance moteur = " << pourcent << "%" << endl;
		//putText(im,"TEST",centre,5,10,0,Scalar(0,0,255));
	}

	/*cout << "x brut = " << posx;
	cout << " | y brut = " << posy << endl;
	cout << "x _kalman = " << estimated.at<float>(0);
	cout << " | y _kalman = " << estimated.at<float>(1) << endl;
	cout << "                               z brut = " << posz;
	cout << "                               | Kalman z = " << estimated.at<float>(2) << endl;*/
	return estimated;
}

