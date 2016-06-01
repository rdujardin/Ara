/*
adjustable.h (part of Ara, https://github.com/rdujardin/Ara)

Copyright (c) 2016, Raphaël Dujardin (rdujardin) & Jean Boehm (jboehm1)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef ADJUSTABLE_H
#define ADJUSTABLE_H

#include <opencv2/opencv.hpp>
#include <string>

#ifdef ADJUSTABLE_C
std::map<std::string,bool> adjustableWindows;
#else
extern std::map<std::string,bool> adjustableWindows;
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

