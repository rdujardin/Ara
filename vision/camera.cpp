/*
camera.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "camera.h"

using namespace cv;
using namespace std;

Camera::Camera(bool adjustable,unsigned int camId) : VideoCapture(camId), Adjustable("Camera Settings") {
	_camId=camId;

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
	ostringstream oss;
	oss << "v4l2-ctl -d /dev/video" << _camId << " " << args;
	return system(oss.str().c_str());
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

