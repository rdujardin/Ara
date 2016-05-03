#include "application.h"

using namespace std;
using namespace cv;

Application::Application(int argc,char* argv[]) {
	readArgs(argc,argv);

	_cam=new Camera(true,_optCamId);
	if (!_cam->isOpened()) throw -1;

	initWindows();

	_ballDetector=new BallDetector(_cam);
	_botController=new BotController(_optWithBot,_optForceWithRoutines);

	while(true) {
		Position pos;
		_ballDetector->loop(pos);
		pos.y+=30;
		//adaptOrientation(pos);
		if(!_botController->loop(pos)) break;

		char key=waitKey(1);
		if(key==27) _botController->nextState();
	}

}

Application::~Application() {
	delete _botController;
	delete _ballDetector;
	delete _cam;
}

void Application::adaptOrientation(Position& pos) {
	double horizAngle=0; //°, angle de la caméra % à l'horizontale, orienté vers le haut     <<<<<<<<<<<< ANGLE HORIZONTALE CAMERA HERE
	double tmpY=pos.y;
	horizAngle=-horizAngle*M_PI/180;
	pos.y=cos(horizAngle)*tmpY+sin(horizAngle)*pos.z;
	pos.z=-sin(horizAngle)*tmpY+cos(horizAngle)*pos.z;
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
	adjustableWindows["Bras (vue de cote)"]=false;

}

void Application::readArgs(int argc,char* argv[]) {
	//Set default options
	_optWithBot=false;
	_optForceWithRoutines=false;
	_optCamId=0;

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
			}
		}
	}
}

