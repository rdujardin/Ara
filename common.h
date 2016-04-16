#ifndef COMMON_H
#define COMMON_H

#include <thread>
#include <mutex>
#include <iostream>
#include <queue>
#include <ctime>
#include "logWindow.h"

enum Mode {
	ALL,
	ONLY_DETECTION,
	ONLY_CONTROL
};

struct Position {
	bool valid;
	double x,y,z;
};

static std::queue<Position> detection;
static LogWindow logs;

#define WORK_W 640
#define WORK_H 480

#endif

