#define COMMON_C
#include "common.h"

using namespace std;

SyncCout::SyncCout() {}

SyncCout& SyncCout::operator()(const int& t) {
	_coutMutex.lock();
	cout << t;
	_coutMutex.unlock();
	return *this;
}

SyncCout& SyncCout::operator()(const double& t) {
	_coutMutex.lock();
	cout << t;
	_coutMutex.unlock();
	return *this;
}

SyncCout& SyncCout::operator()(const char& t) {
	_coutMutex.lock();
	cout << t;
	_coutMutex.unlock();
	return *this;
}

SyncCout& SyncCout::operator()(const string& t) {
	_coutMutex.lock();
	cout << t;
	_coutMutex.unlock();
	return *this;
}

SyncCout& operator<<(SyncCout& out,int v) {
	return out(v);
}
SyncCout& operator<<(SyncCout& out,double v) {
	return out(v);
}
SyncCout& operator<<(SyncCout& out,char v) {
	return out(v);
}
SyncCout& operator<<(SyncCout& out,string v) {
	return out(v);
}

