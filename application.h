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
		Camera* _cam;
		BallDetector* _ballDetector;
		BotController* _botController;

		void adaptOrientation(Position& pos);
		void initWindows();
};

#endif

