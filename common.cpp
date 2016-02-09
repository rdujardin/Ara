#define COMMON_C
#include "common.h"

SyncOutputStream::SyncOutputStream(std::ostream& out) {
	_out=&out;
}

SyncOutputStream& operator<<(SyncOutputStream& out,int v) {
	out._mutex.lock();
	(*(out._out)) << v;
	out._mutex.unlock();
	return out;
}

SyncOutputStream& operator<<(SyncOutputStream& out,double v) {
	out._mutex.lock();
	(*(out._out)) << v;
	out._mutex.unlock();
	return out;
}

SyncOutputStream& operator<<(SyncOutputStream& out,std::string v) {
	out._mutex.lock();
	(*(out._out)) << v;
	out._mutex.unlock();
	return out;
}

