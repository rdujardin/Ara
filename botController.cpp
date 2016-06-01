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

	_state.set(cartesian,terminalX,0,terminalY,35,terminalZ,22);

	_vehicleLeftSpeed=0;
	_vehicleRightSpeed=0;

	_params["X = "]=_state.get(terminalX)*2+70;
	_params["Y = "]=_state.get(terminalY);
	_params["Z = "]=_state.get(terminalZ)*2+40;
	
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
	loopAngles(true);
	_trajIt++;
	Timer::wait(20);
	return ret;
}

bool BotController::follow(Position detection) {

	_state.set(cartesian,terminalX,detection.x,terminalY,detection.y,terminalZ,detection.z);
	loopAngles();
	return true;

}

bool BotController::loopGather(Position detection) {
	//TODO
	return true;
}

bool BotController::loopManual() {
	loopAngles();
	return true;
}

bool BotController::loopAngles(bool unsafe) {
	logs["Computed1"].reset() << "Computed angles ## ";
	logs["Computed2"].reset() << "Theta 0 : " << _state.get(theta0)*180/M_PI;
	logs["Computed3"].reset() << "Alpha 1 : " << _state.get(alpha1)*180/M_PI;
	logs["Computed4"].reset() << "Alpha 2 : " << _state.get(alpha2)*180/M_PI;
	logs["Computed5"].reset() << "Alpha 3 : " << _state.get(alpha3)*180/M_PI;
	logs["Computed6"].reset() << "Theta 3 : " << _state.get(theta3)*180/M_PI;
	logs.refresh();

	Mat draw1=Mat::zeros(BotDraw::_drawHeight,BotDraw::_drawWidth,CV_8UC3);
	Mat draw2=Mat::zeros(BotDraw::_drawHeight,BotDraw::_drawWidth,CV_8UC3);

	//VÉRIFICATION DE LA ZONE DE TRAVAIL
	bool workZoneCheck=_state.checkWorkingZone();

	//Draw axis
	BotDraw::drawAxis(draw1,draw2);
	
	//Draw bot
	BotDraw::drawBot(draw1,draw2,_state,workZoneCheck || unsafe);

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
		_state.set(cartesian,terminalX,(_params["X = "]-70)/2,terminalY,_params["Y = "],terminalZ,(_params["Z = "]-40)/2);
		loopAngles();
	}
}
