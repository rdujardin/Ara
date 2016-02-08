#ifndef ADJUSTABLE_H
#define ADJUSTABLE_H

#include <opencv2/opencv.hpp>
#include <string>

#ifdef ADJUSTABLE_C
std::map<std::string,bool> activatedWindows;
#else
extern std::map<std::string,bool> activatedWindows;
#endif

class Adjustable;

struct CallbackParam {
	CallbackParam(std::string name,Adjustable* owner) {
		_name=name;
		_owner=owner;
	}
	
	std::string _name;
	Adjustable* _owner;
};

class Adjustable {
public:
	Adjustable(std::string window="Settings");
	~Adjustable();
	static void callback(int val,void* data);
	std::map<std::string,int> _params;
protected:
	void makeAdjustable(std::string name,int count);
	virtual void adjusted(std::string name,int val)=0;
private:
	std::map<std::string,CallbackParam*> _callbackParams;
	std::string _window;
};

#endif

