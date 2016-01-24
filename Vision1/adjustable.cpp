#include "adjustable.h"

using namespace cv;
using namespace std;

Adjustable::Adjustable(std::string window) {
	_window=window;
}

Adjustable::~Adjustable() {
	for(map<string,CallbackParam*>::iterator it=_callbackParams.begin();it!=_callbackParams.end();++it) {
		delete it->second;
	}
}

void Adjustable::makeAdjustable(string name,int count) {
	_callbackParams[name]=new CallbackParam(name,this);
	createTrackbar(name,_window,&(_params[name]),count,&Adjustable::callback,_callbackParams[name]);
}

void Adjustable::callback(int val,void *data) {
	CallbackParam* param=(CallbackParam*) data;
	param->_owner->adjusted(param->_name,val);
}

