#ifndef COMMON_H
#define COMMON_H

#include <thread>
#include <mutex>
#include <iostream>
#include <queue>
#include <ctime>
#include "logWindow.h"

enum Mode {
	FOLLOW,
	GATHER,
	MANUAL,
	STARTUP,
	SHUTDOWN
};

struct Position {
	bool valid;
	double x,y,z;
};

static std::queue<Position> detection;

#define WORK_W 640
#define WORK_H 480

#endif

