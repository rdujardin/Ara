#ifndef APPLICATION_H
#define APPLICATION_H

#include <cstring>
#include <iostream>

#include "common.h"
#include "ballDetector.h"
#include "botController.h"
#include "logWindow.h"

class Application {
	public:
		Application(int argc,char* argv[]);
		~Application();

	private:
		void setMode(Mode mode);
		bool testNextMode();
		bool nextMode();

		Mode _mode,_optMode;

		Camera* _cam;
		BallDetector* _ballDetector;
		BotController* _botController;

		bool _optWithBot;
		bool _optForceWithRoutines;
		unsigned int _optCamId;
		bool _optWithPosSliders;

		void readArgs(int argc,char* argv[]);
		void adaptPosition(Position& pos);
		void adaptOrientation(Position& pos);
		void initWindows();
};

#endif

