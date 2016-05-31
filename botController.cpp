/*
botController.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "botController.h"

using namespace cv;
using namespace std;

BotController::BotController(bool withBot) : Adjustable("Bras (vue de cote)") {

	_withBot=withBot;
	_hardware=new BotHardware(withBot);

	_state.terminalX=0;
	_state.terminalY=35;
	_state.terminalZ=22;

	_vehicleLeftSpeed=0;
	_vehicleRightSpeed=0;

	_params["X = "]=_state.terminalX*2+70;
	_params["Y = "]=_state.terminalY;
	_params["Z = "]=_state.terminalZ*2+40;
	
	makeAdjustable("X = ",140);
	makeAdjustable("Y = ",70);
	makeAdjustable("Z = ",140);

	_hardware->checkBatteryLevel();
}

BotController::~BotController() {
	delete _hardware;
}

void BotController::setMode(Mode mode) {
	_mode=mode;
	if(mode==STARTUP) BotTrajectories::initStartUpRoutine(_trajectory,_trajIt,_state);
	else if(mode==PRESHUTDOWN) BotTrajectories::initPreShutDownRoutine(_trajectory,_trajIt,_state);
	else if(mode==SHUTDOWN) BotTrajectories::initShutDownRoutine(_trajectory,_trajIt,_state);
	else return;
}

bool BotController::loopRoutine() {
	bool ret;
	if(_mode==STARTUP) ret=BotTrajectories::loopStartUpRoutine(_state,_trajectory,_trajIt);
	else if(_mode==PRESHUTDOWN) ret=BotTrajectories::loopPreShutDownRoutine(_state,_trajectory,_trajIt);
	else if(_mode==SHUTDOWN) ret=BotTrajectories::loopShutDownRoutine(_state,_trajectory,_trajIt);
	loopAngles();
	Timer::wait(20);
	return ret;
}

bool BotController::follow(Position detection) {

	_state.terminalX=detection.x;
	_state.terminalY=detection.y;
	_state.terminalZ=detection.z;
	_state=BotTrajectories::computeAngles(_state);
	loopAngles();
	return true;

}

bool BotController::loopGather(Position detection) {
	//TODO
	return true;
}

bool BotController::loopManual() {
	manual();	
	return true;
}

bool BotController::checkWorkingZone() {
	bool w=true;
	w=w && _state.theta0>=0*M_PI/180 && _state.theta0<=140*M_PI/180;
	w=w && _state.alpha1>=0*M_PI/180 && _state.alpha1<=140*M_PI/180;
	w=w && _state.alpha2>=0*M_PI/180 && _state.alpha2<=180*M_PI/180;
	w=w && _state.alpha3>=0*M_PI/180 && _state.alpha3<=180*M_PI/180;
	w=w && _state.theta3>=0*M_PI/180 && _state.theta3<=180*M_PI/180;

	w=w && _state.terminalY>0+_terminalYOffset;
	w=w && _state.terminalZ>=20;
	w=w && _state.terminalZ<45;

	w=w && _state.wristY>0;

	return w;
}

bool BotController::loopAngles() {
	Mat draw1=Mat::zeros(BotDraw::_drawHeight,BotDraw::_drawWidth,CV_8UC3);
	Mat draw2=Mat::zeros(BotDraw::_drawHeight,BotDraw::_drawWidth,CV_8UC3);

	//VÉRIFICATION DE LA ZONE DE TRAVAIL
	bool workZoneCheck=checkWorkingZone();

	//Draw axis
	BotDraw::drawAxis(draw1,draw2);
	
	//Draw bot
	BotDraw::drawBot(draw1,draw2,_state,workZoneCheck);

	cout << "." << endl;
	
	imshow("Bras (vue de cote)",draw1);
	imshow("Bras (vue de haut)",draw2);

	//double angle_test(__alpha0);
	_hardware->checkBatteryLevel();
	if(workZoneCheck) _hardware->sendToMotors(_state);
	
	return true;
}

void BotController::adjusted(std::string name,int val) {
	if(_mode==MANUAL) {
		_params[name]=val;
		_state.terminalX=(_params["X = "]-70)/2;
		_state.terminalY=_params["Y = "];
		_state.terminalZ=(_params["Z = "]-40)/2;
		manual();
	}
}

void BotController::manual() {
	_state=BotTrajectories::computeAngles(_state);
	loopAngles();
}
