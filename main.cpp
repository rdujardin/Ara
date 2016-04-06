#include <cstring>
#include <iostream>

#include "common.h"
#include "ballDetector.h"
#include "botController.h"

using namespace std;

void readArgs(int argc,char* argv[],Mode& mode,bool& withBot);

int main(int argc,char* argv[]) {
	Mode mode;
	bool withBot;
	readArgs(argc,argv,mode,withBot);
	
	activatedWindows["Settings"]=(mode==ONLY_DETECTION);
	activatedWindows["Camera Settings"]=(mode==ONLY_DETECTION);
	activatedWindows["Bras (vue de cote)"]=(mode!=ONLY_DETECTION);
	activatedWindows["Bras (vue de haut)"]=(mode!=ONLY_DETECTION);

	BallDetector* ballDetector=NULL;
	if(mode!=ONLY_CONTROL) ballDetector=new BallDetector(mode,withBot,mode!=ONLY_CONTROL,mode==ONLY_DETECTION,mode==ONLY_DETECTION,mode!=ONLY_CONTROL,mode==ONLY_DETECTION);
	BotController* botController=new BotController(mode,withBot,mode!=ONLY_DETECTION,mode==ONLY_CONTROL);

	while(true) {
		Position pos;
		if(mode!=ONLY_CONTROL) if(!ballDetector->loop(pos)) break;
		cout << "####POSITION(cam) " << pos.x << " / " << pos.y << " / " << pos.z << endl;
		double horizAngle=0; //°
		//Horizon rotation :
		double tmpY=pos.y;
		horizAngle=horizAngle*M_PI/180;
		pos.y=cos(horizAngle)*tmpY-sin(horizAngle)*pos.z;
		pos.z=sin(horizAngle)*tmpY+cos(horizAngle)*pos.z;
		cout << "####POSITION(bot) " << pos.x << " / " << pos.y << " / " << pos.z << endl;
		if(!botController->loop(pos)) break;
	}

	delete botController;
	if(ballDetector) delete ballDetector;

	return 0;
}

void readArgs(int argc,char* argv[],Mode& mode,bool& withBot) {
	mode=ALL;
	withBot=false;
	for(unsigned int i=1;i<argc;i++) {
		if(strcmp(argv[i],"detection")==0) mode=ONLY_DETECTION;
		else if(strcmp(argv[i],"control")==0) mode=ONLY_CONTROL;
		else if(strcmp(argv[i],"bot")==0) withBot=true;
	}
	cout << "PROVOS > Hello." << endl;
	cout << "PROVOS > Mode " << string((mode==ALL)?"detection & control":((mode==ONLY_DETECTION)?"detection only":"control only")) << " , " << string(withBot?"with":"without") << " bot." << endl;
	cout << "-------------------------------------------------" << endl;
}

