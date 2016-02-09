#ifndef COMMON_H
#define COMMON_H

#include <thread>
#include <mutex>
#include <iostream>

struct Position {
	double x,y,z;
};

#ifdef COMMON_C
	std::mutex detectionMutex;
#else
	extern std::mutex detectionMutex;
#endif

class SyncOutputStream {
	public:
		SyncOutputStream(std::ostream& out);

		static std::mutex _mutex;
		std::ostream* _out;
};

SyncOutputStream& operator<<(SyncOutputStream& out,int v);
SyncOutputStream& operator<<(SyncOutputStream& out,double v);
SyncOutputStream& operator<<(SyncOutputStream& out,std::string v);

#ifdef COMMON_C
	SyncOutputStream syncCout(std::cout);
#else
	extern SyncOutputStream syncCout;
#endif

#endif

