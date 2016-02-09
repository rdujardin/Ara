#include <cstring>
#include <iostream>
#include <queue>

#include "common.h"
#include "ballDetector.h"
#include "botController.h"

using namespace std;

enum Mode {
	ALL,
	ONLY_DETECTION,
	ONLY_CONTROL
};

void readArgs(int argc,char* argv[],Mode& mode,bool& withBot);

int main(int argc,char* argv[]) {
	Mode mode;
	bool withBot;
	readArgs(argc,argv,mode,withBot);
	
	activatedWindows["Settings"]=(mode==ONLY_DETECTION);
	activatedWindows["Camera Settings"]=(mode==ONLY_DETECTION);
	activatedWindows["Bras (vue de cote)"]=(mode!=ONLY_DETECTION);
	activatedWindows["Bras (vue de haut)"]=(mode!=ONLY_DETECTION);

	BallDetector ballDetector(mode!=ONLY_CONTROL,mode==ONLY_DETECTION,mode==ONLY_DETECTION,mode!=ONLY_CONTROL,mode==ONLY_DETECTION);
	BotController botController(withBot,mode!=ONLY_DETECTION,mode==ONLY_CONTROL);

	queue<Position> detection;
	bool running=true;

	thread t_ballDetector(&BallDetector::run,&ballDetector,detection,running);
	thread t_botController(&BotController::run,&botController,detection,running);

	t_botController.join();
	t_ballDetector.join();

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

