/*
application.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "application.h"

using namespace std;
using namespace cv;

Application::Application(int argc,char* argv[]) {
	_pause=false;
	_gatherGo=false;
	readArgs(argc,argv);
	_cam=new Camera(true,_optCamId);
	if (!_cam->isOpened()) throw -1;
	_mode=(_optWithBot || _optForceWithRoutines)?STARTUP:_optMode;
	initWindows();
	_ballDetector=new BallDetector(_cam,_optBall);
	_botController=new BotController(_optWithBot);
	_botController->setMode(_mode);

	while(true) {
		int event=checkKeyboard();
		if(!_pause) {
			cout << "<" << _botController->state().stringify() << ">" << endl;
			if(_mode==STARTUP) {
				cout << "STARTUP" << endl;
				if(!_botController->loopRoutine()) setMode(_optMode);
			}
			else if(_mode==FOLLOW) {
				cout << "FOLLOW" << endl;
				Position pos;
				_ballDetector->loop(pos);
				adaptPosition(pos);
				cout << "X " << pos.x << " ; Y " << pos.y << " ; Z " << pos.z << endl;
				if(pos.valid) _botController->follow(pos);

				if(event==EVENT_EXIT) break;
			}
			else if(_mode==GATHER) {
				cout << "GATHER" << endl;
				Position pos;
				_ballDetector->loop(pos);
				adaptPosition(pos);
				if(pos.valid && _gatherGo) {
					BotState bs;
					bs.set(cartesian,terminalX,pos.x,terminalY,pos.y,terminalZ,pos.z);
					if(bs.checkWorkingZone()) {
						_gatherGo=false;
						_botController->gather(pos);
						setMode(TRAJECTORY,GATHER_GRAB);
					}
				}
				
				if(event==EVENT_EXIT) break;
			}
			else if(_mode==TRAJECTORY) {
				cout << "TRAJECTORY" << endl;
				if(!_botController->trajectory()) setMode(_nextMode);
				if(event==EVENT_EXIT) break;
			}
			else if(_mode==GATHER_GRAB) {
				cout << "GATHER_GRAB" << endl;
				if(!_botController->gatherGrab()) setMode(GATHER_BACK);
				if(event==EVENT_EXIT) break;
			}
			else if(_mode==GATHER_BACK) {
				cout << "GATHER_BACK" << endl;
				_botController->gatherBack();
				setMode(TRAJECTORY,GATHER_RELEASE);
				if(event==EVENT_EXIT) break;
			}
			else if(_mode==GATHER_RELEASE) {
				cout << "GATHER_RELEASE" << endl;
				_botController->gatherRelease();
				setMode(GATHER);
				if(event==EVENT_EXIT) break;
			}
			else if(_mode==MANUAL) {
				cout << "MANUAL" << endl;
				_botController->loopManual();

				if(event==EVENT_EXIT) break;
			}
			else if(_mode==PRESHUTDOWN) {
				cout << "PRESHUTDOWN" << endl;
				if(!_botController->loopRoutine()) setMode(SHUTDOWN);
			}
			else if(_mode==SHUTDOWN) {
				cout << "SHUTDOWN" << endl;
				if(!_botController->loopRoutine()) break;
			}
			else { //NULL_MODE
				cout << "NULLMODE" << endl;
				if(event==EVENT_EXIT) break;
			}
		}
		logs.processRepainting();
	}

}

Application::~Application() {
	delete _botController;
	delete _ballDetector;
	delete _cam;
}

int Application::checkKeyboard() {
	int key=waitKey(1);
	ostringstream oss;
	oss << key;
	cout << "KKKKKKK " << oss.str() << endl;
	if(key==KEY_SPACE) {
		_pause=true;
		return EVENT_CONTINUE;
	}
	else if(key==KEY_BACKSPACE) {
		_pause=false;
		return EVENT_CONTINUE;
	}
	else {
		if(_mode==STARTUP || _mode==PRESHUTDOWN || _mode==SHUTDOWN) return EVENT_CONTINUE; 
		else if(_mode==NULL_MODE || _mode==FOLLOW || _mode==GATHER || _mode==GATHER_BACK || _mode==TRAJECTORY || _mode==GATHER_GRAB || _mode==GATHER_RELEASE || _mode==MANUAL) {
			if(key==KEY_ENTER && _mode==GATHER) _gatherGo=true;
			else if(key==KEY_ESCAPE) {
				cout << "ESCAPE !!!!!!!!!! " << endl;
				if(_optWithBot || _optForceWithRoutines) {
					setMode(PRESHUTDOWN);
					return EVENT_CONTINUE;
				}
				else return EVENT_EXIT;
			}
			else return EVENT_CONTINUE;
		}
	}
}

void Application::adaptPosition(Position& pos) {
	cout << "ADAPT1 " << pos.x << " ; " << pos.y << " ; " << pos.z << endl;
	pos.y+=offset_y_camera;
	pos.z+=offset_z_camera+BallDetector::ballRadius/2;
	adaptOrientation(pos);
}

void Application::adaptOrientation(Position& pos) {
	double horizAngle=45-atan(offset_y_camera/_optCoordCalib)*180/M_PI; //°, angle de la caméra % à l'horizontale, orienté vers le haut     <<<<<<<<<<<< ANGLE HORIZONTALE CAMERA HERE
	cout << "angle : " << horizAngle << endl;
	double tmpY=pos.y;
	horizAngle=horizAngle*M_PI/180;
	pos.y=cos(horizAngle)*tmpY+sin(horizAngle)*pos.z;
	pos.z=-sin(horizAngle)*tmpY+cos(horizAngle)*pos.z;
	cout << "ADAPT2 " << pos.x << " ; " << pos.y << " ; " << pos.z << endl;
}

void Application::setMode(Mode mode,Mode next) {
	_nextMode=next;
	_mode=mode;
	_botController->setMode(mode);
}

void Application::initWindows() {
	namedWindow("Settings", 3);
	moveWindow("Settings",200,40);

	/*cv::namedWindow("Camera Settings", 5);
	moveWindow("Camera Settings",840,40);*/
	
	namedWindow("Input", 1);
	namedWindow("Output", 1);
	namedWindow("Trajectory",1);
	moveWindow("Input",0,0);
	moveWindow("Output",700,0);
	moveWindow("Trajectory",1300,0);

	namedWindow("Bras (vue de cote)",1);
	namedWindow("Bras (vue de haut)",1);
	moveWindow("Bras (vue de cote)",0,600);
	moveWindow("Bras (vue de haut)",740,600);

	namedWindow("Logs",1);
	moveWindow("Logs",1400,580);
	
	adjustableWindows["Settings"]=true;
	adjustableWindows["Camera Settings"]=false;
	adjustableWindows["Bras (vue de cote)"]=(_optMode==MANUAL);

}

void Application::readArgs(int argc,char* argv[]) {
	//Set default options
	_optWithBot=false;
	_optForceWithRoutines=false;
	_optCamId=1;
	_optMode=GATHER;
	_optBall="green";
	_optCoordCalib=60.0;

	//Read arguments
	vector<string> args;
	for(unsigned int c=1;c<argc;++c) {
		args.push_back(string(argv[c]));
	}

	for(vector<string>::iterator it=args.begin();it!=args.end();++it) {
		string arg=*it;
		
		if(arg=="bot") _optWithBot=true;
		else if(arg=="routines") _optForceWithRoutines=true;
		else {
			size_t equal=arg.find_first_of("=");
			if(equal<arg.size()-1) {
				string option=arg.substr(0,equal);
				string value=arg.substr(equal+1);

				if(option=="cam") _optCamId=stoi(value);
				else if(option=="mode") {
					if(value=="gather") _optMode=GATHER;
					else if(value=="follow") _optMode=FOLLOW;
					else if(value=="manual") _optMode=MANUAL;
				}
				else if(option=="ball") _optBall=value;
				else if(option=="std") _optCoordCalib=stof(value);
			}
		}
	}
}

