#ifndef COMMON_H
#define COMMON_H

#include <thread>
#include <mutex>
#include <iostream>
#include <queue>

struct Position {
	double x,y,z;
};

#ifdef COMMON_C
	std::mutex detectionMutex;
#else
	extern std::mutex detectionMutex;
#endif

static std::mutex _coutMutex;

class SyncCout {
	public:
		SyncCout();
		
		SyncCout& operator()(const int& t);
		SyncCout& operator()(const double& t);
		SyncCout& operator()(const char& t);
		SyncCout& operator()(const std::string& t);


	private:
		
};

SyncCout& operator<<(SyncCout& out,int v);
SyncCout& operator<<(SyncCout& out,double v);
SyncCout& operator<<(SyncCout& out,char v);
SyncCout& operator<<(SyncCout& out,std::string v);

static SyncCout syncCout;
static char syncEndl='\n';

static std::queue<Position> detection;

#endif
