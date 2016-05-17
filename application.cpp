#include "application.h"

using namespace std;
using namespace cv;

Application::Application(int argc,char* argv[]) {
	readArgs(argc,argv);

	_cam=new Camera(true,_optCamId);
	if (!_cam->isOpened()) throw -1;

	_mode=(_optWithBot || _optForceWithRoutines)?STARTUP:_optMode;

	initWindows();

	_ballDetector=new BallDetector(_cam);
	_botController=new BotController(_optWithBot);

	_botController->setMode(_mode);

	while(true) {
		if(_mode==STARTUP) {
			if(!_botController->loopStartUpRoutine()) setMode(_optMode);
			waitKey(1);
		}
		else if(_mode==FOLLOW) {
			Position pos;
			_ballDetector->loop(pos);
			adaptPosition(pos);
			if(!_botController->follow(pos)) {
				if(!nextMode()) break;
			}

			if(!testNextMode()) break;
		}
		else if(_mode==GATHER) {
			Position pos;
			_ballDetector->loop(pos);
			adaptPosition(pos);
			if(!_botController->loopGather(pos)) {
				if(!nextMode()) break;
			}
			
			if(!testNextMode()) break;
		}
		else if(_mode==MANUAL) {
			if(!_botController->loopManual()) {
				if(!nextMode()) break;
			}

			if(!testNextMode()) break;
		}
		else { //SHUTDOWN
			if(!_botController->loopShutDownRoutine()) break;
			waitKey(1);
		}
	}

}

Application::~Application() {
	delete _botController;
	delete _ballDetector;
	delete _cam;
}

void Application::adaptPosition(Position& pos) {
	pos.y+=30;
	//adaptOrientation(pos);
}

void Application::adaptOrientation(Position& pos) {
	double horizAngle=0; //°, angle de la caméra % à l'horizontale, orienté vers le haut     <<<<<<<<<<<< ANGLE HORIZONTALE CAMERA HERE
	double tmpY=pos.y;
	horizAngle=-horizAngle*M_PI/180;
	pos.y=cos(horizAngle)*tmpY+sin(horizAngle)*pos.z;
	pos.z=-sin(horizAngle)*tmpY+cos(horizAngle)*pos.z;
}

void Application::setMode(Mode mode) {
	_mode=mode;
	_botController->setMode(mode);
}

bool Application::testNextMode() {
	char key=waitKey(1);
	if(key==27) return nextMode();
	return true;
}

bool Application::nextMode() {
	if(_optWithBot || _optForceWithRoutines) {
		setMode(SHUTDOWN);
		return true;
	}
	else return false;
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
	_optMode=FOLLOW;

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
					else _optMode=MANUAL;
				}
			}
		}
	}
}
