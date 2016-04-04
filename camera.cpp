#include "camera.h"

using namespace cv;
using namespace std;

Camera::Camera(bool adjustable) : VideoCapture(1/*j1*/), Adjustable("Camera Settings") {
	set(CV_CAP_PROP_FRAME_WIDTH, width);
	set(CV_CAP_PROP_FRAME_HEIGHT, height);
	set(CV_CAP_PROP_FPS, framerate);
	
	_params["brightness"]=128;
	_params["contrast"]=32;
	_params["saturation"]=64; //j128
	_params["white_balance_temperature_auto"]=0;
	_params["gain"]=37; //0
	_params["power_line_frequency"]=1;
	_params["white_balance_temperature"]=4000;
	_params["sharpness"]=0;
	_params["backlight_compensation"]=0;
	_params["exposure_auto"]=1; //1 : manual mode
	_params["exposure_auto_priority"]=0; //j0
	_params["exposure_absolute"]=200; //night:400
	
	
	updateV4lAll();
	
	if(adjustable) {
		for(map<string,int>::iterator it=_params.begin();it!=_params.end();++it) {
			if(it->first=="white_balance_temperature_auto" || it->first=="backlight_compensation" || it->first=="exposure_auto_priority") makeAdjustable(it->first,1); //bool
			else if(it->first=="power_line_frequency") makeAdjustable(it->first,2); //menu
			else if(it->first=="exposure_auto") makeAdjustable(it->first,3); //menu
			else if(it->first=="white_balance_temperature" || it->first=="exposure_absolute") makeAdjustable(it->first,10000); //int
			else makeAdjustable(it->first,255);
		}
	}
}

int Camera::v4l(string args) {
	string str="v4l2-ctl -d /dev/video1 "+args;
	return system(str.c_str());
}

int Camera::updateV4l(string name) {
	ostringstream oss;
	oss << "-c " << name << "=" << _params[name];
	return v4l(oss.str());
}

int Camera::updateV4lAll() {
	int ret=0;
	for(map<string,int>::iterator it=_params.begin();it!=_params.end();++it) {
		if(updateV4l(it->first)!=0) ret++;
	}
	return ret;
}

void Camera::adjusted(std::string name,int val) {
	_params[name]=val;
	updateV4l(name);
}

