/*
botHardware.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "botHardware.h"

using namespace std;

BotHardware::BotHardware(bool withBot) {
	_withBot=withBot;
	_batteryLevel=-1;
	if(_withBot)
		if((_fd=serialOpen("/dev/ttyUSB0"/*0*/,9600))<0) throw -1;
}

void BotHardware::sendInt(int v) {
	if(_withBot) serialPutchar(_fd,v);
}

void BotHardware::sendAngle(double angle) {
	int ang=(int) angle;
	cout << ang << ", ";
	sendInt(ang);
}

void BotHardware::sendToMotors(BotState& state) {
	cout << "## SENT " << string(_withBot?"(really)":"(virtually)") << " ";

	sendInt(250);	
	sendAngle(state.get(theta0)*180/M_PI*180/140);
	sendAngle(state.get(alpha1)*180/M_PI*180/140);
	sendAngle(state.get(alpha2)*180/M_PI);
	sendAngle(state.get(alpha3)*180/M_PI);
	sendAngle(state.get(theta3)*180/M_PI);

	cout << endl;

	delay(30);
}

void BotHardware::sendToVehicle(int vehicleLeftSpeed,int vehicleRightSpeed) {
	sendInt(251);
	sendInt(128+vehicleLeftSpeed);
	sendInt(128+vehicleRightSpeed);
	delay(30);
}

void BotHardware::checkBatteryLevel() {
	if(_withBot) {
		char ret[100];
		ret[0]=0;
		int i=0;
		while (serialDataAvail (_fd))
		{
	  		ret[i]= serialGetchar (_fd) ;
			i++;
		}
		ret[i]=0;
		int val=-1;
		if(strlen(ret)>0) val=ret[0];
		if(val<=100) _batteryLevel=val;
		else _batteryLevel=-2;
	}
}

int BotHardware::batteryLevel() {
	return _batteryLevel;
}
