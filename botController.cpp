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

const Point BotController::_drawOrigin=Point(200,425);

void copyState(BotState& dst,BotState& src) {
	dst.alpha1=src.alpha1;
	dst.alpha2=src.alpha2;
	dst.alpha3=src.alpha3;
	dst.theta0=src.theta0;
	dst.theta3=src.theta3;
	dst.terminalX=src.terminalX;
	dst.terminalY=src.terminalY;
	dst.terminalZ=src.terminalZ;
	dst.wristX=src.wristX;
	dst.wristY=src.wristY;
	dst.terminalXTh=src.terminalXTh;
	dst.length3Al=src.length3Al;
}

BotController::BotController(bool withBot) : Adjustable("Bras (vue de cote)") {

	_withBot=withBot;

	_state.terminalX=0;
	_state.terminalY=35;
	_state.terminalZ=22;

	_vehicleLeftSpeed=0;
	_vehicleRightSpeed=0;

	if(_withBot)
		if(!initSerial()) throw -1;

	_params["X = "]=_state.terminalX*2+70;
	_params["Y = "]=_state.terminalY;
	_params["Z = "]=_state.terminalZ*2+40;
	
	makeAdjustable("X = ",140);
	makeAdjustable("Y = ",70);
	makeAdjustable("Z = ",140);

	_batteryLevel=-1;
	checkBatteryLevel();
}

BotController::~BotController() {}

void BotController::setMode(Mode mode) {
	_mode=mode;
	if(mode==STARTUP) initStartUpRoutine();
	else if(mode==PRESHUTDOWN) initPreShutDownRoutine();
	else if(mode==SHUTDOWN) initShutDownRoutine();
	else return;
}

bool BotController::follow(Position detection) {

	_state.terminalX=detection.x;
	_state.terminalY=detection.y;
	_state.terminalZ=detection.z;
	_state=computeAngles(_state);

	loopAngles();
	checkBatteryLevel();
	return true;

}

bool BotController::loopGather(Position detection) {
	//TODO
	checkBatteryLevel();
	return true;
}

bool BotController::loopManual() {
	manual();	
	checkBatteryLevel();
	return true;
}

BotState BotController::computeAngles(BotState state) {
	//Angles computing :
	state.theta0=atan2((state.terminalZ-_length3*cos(_terminalAbsAlpha)*cos(_terminalAbsTheta)),(state.terminalX+_length3*cos(_terminalAbsAlpha)*sin(_terminalAbsTheta)));
	state.theta3=M_PI/2-_terminalAbsTheta-state.theta0;
	state.length3Al=_length3*cos(state.theta3);
	state.wristX=(state.terminalX+_length3*cos(_terminalAbsAlpha)*sin(_terminalAbsTheta))/cos(state.theta0);
	state.wristY=state.terminalY-_length3*sin(_terminalAbsAlpha);
	state.terminalXTh=state.wristX+state.length3Al*cos(_terminalAbsAlpha);

	//cf 379.pdf :
	double cosAlpha2=(state.wristX*state.wristX+state.wristY*state.wristY-_length1*_length1-_length2*_length2)/(2*_length1*_length2);
	double sinAlpha2=-sqrt(1-cosAlpha2*cosAlpha2);
	double cosAlpha1=(state.wristX*(_length1+_length2*cosAlpha2)+state.wristY*_length2*sinAlpha2)/(state.wristX*state.wristX+state.wristY*state.wristY);
	double sinAlpha1=sqrt(1-cosAlpha1*cosAlpha1);
	state.alpha1=atan2(sinAlpha1,cosAlpha1);
	state.alpha2=atan2(sinAlpha2,cosAlpha2);
	state.alpha3=_terminalAbsAlpha-state.alpha1-state.alpha2;

	logs["Computed1"].reset() << "Computed angles ## ";
	logs["Computed2"].reset() << "Theta 0 : " << state.theta0*180/M_PI;
	logs["Computed3"].reset() << "Alpha 1 : " << state.alpha1*180/M_PI;
	logs["Computed4"].reset() << "Alpha 2 : " << state.alpha2*180/M_PI;
	logs["Computed5"].reset() << "Alpha 3 : " << state.alpha3*180/M_PI;
	logs["Computed6"].reset() << "Theta 3 : " << state.theta3*180/M_PI;
	logs.refresh();

	//Conversion : computed->real
	state.theta0=conv(0,true,state.theta0);
	state.alpha1=conv(1,true,state.alpha1);
	state.alpha2=conv(2,true,state.alpha2);
	state.alpha3=conv(3,true,state.alpha3);
	state.theta3=conv(4,true,state.theta3);

	return state;
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
	Mat draw1=Mat::zeros(_drawHeight,_drawWidth,CV_8UC3);
	Mat draw2=Mat::zeros(_drawHeight,_drawWidth,CV_8UC3);

	//VÉRIFICATION DE LA ZONE DE TRAVAIL
	bool workZoneCheck=checkWorkingZone();

	//Draw axis
	drawAxis(draw1,draw2);
	
	//Draw bot
	drawBot(draw1,draw2,workZoneCheck);

	cout << "." << endl;
	
	imshow("Bras (vue de cote)",draw1);
	imshow("Bras (vue de haut)",draw2);

	//double angle_test(__alpha0);
	if(workZoneCheck) sendToMotors();
	
	return true;
}

void BotController::drawAxis(Mat& draw1,Mat& draw2) {
	line(draw1,Point(_drawOrigin.x,0),Point(_drawOrigin.x,_drawHeight),Scalar(255,255,255),1);
	line(draw1,Point(0,_drawOrigin.y),Point(_drawWidth,_drawOrigin.y),Scalar(255,255,255),1);
	putText(draw1,"y",Point(_drawOrigin.x+10,20),0,1,Scalar(255,255,255),1);
	putText(draw1,"xTh",Point(_drawWidth-60,_drawOrigin.y-10),0,1,Scalar(255,255,255),1);
	putText(draw1,"Vue de cote",Point(10,_drawHeight-20),2,1,Scalar(0,255,255),2);
	
	line(draw2,Point(_drawOrigin.x,0),Point(_drawOrigin.x,_drawHeight),Scalar(255,255,255),1);
	line(draw2,Point(0,_drawOrigin.y),Point(_drawWidth,_drawOrigin.y),Scalar(255,255,255),1);
	putText(draw2,"z",Point(_drawOrigin.x+10,20),0,1,Scalar(255,255,255),1);
	putText(draw2,"x",Point(_drawWidth-20,_drawOrigin.y-10),0,1,Scalar(255,255,255),1);
	putText(draw2,"Vue de dessus",Point(10,_drawHeight-20),2,1,Scalar(0,255,255),2);

	ostringstream oss;
	oss << "BATTERY ";
	if(_batteryLevel>=0) oss << _batteryLevel << "%";
	else if(_batteryLevel==-1) oss << "n.c.";
	else oss << "TOO LOW";
	putText(draw1,oss.str(),Point(_drawWidth-128,_drawHeight-25),1,1,Scalar(0,255,255),1);
}

void BotController::drawBot(Mat& draw1,Mat& draw2,bool workZoneCheck) {
	double alpha1=conv(1,false,_state.alpha1);
	double alpha2=conv(2,false,_state.alpha2);
	double alpha3=conv(3,false,_state.alpha3);
	double theta0=conv(0,false,_state.theta0);
	double theta3=conv(4,false,_state.theta3);

	Point p1(_drawOrigin.x+_drawScale*_length1*cos(alpha1),_drawOrigin.y-_drawScale*_length1*sin(alpha1));
	Point p2(p1.x+_drawScale*_length2*cos(alpha2+alpha1),p1.y-_drawScale*_length2*sin(alpha2+alpha1));
	Point p3(p2.x+_drawScale*_state.length3Al*cos(alpha1+alpha2+alpha3),p2.y-_drawScale*_state.length3Al*sin(alpha1+alpha2+alpha3));
	
	Point pp1(_drawOrigin.x+_drawScale*_state.wristX*cos(theta0),_drawOrigin.y-_drawScale*_state.wristX*sin(theta0));
	Point pp2(pp1.x+_drawScale*_length3Th*cos(theta0+theta3),pp1.y-_drawScale*_length3Th*sin(theta0+theta3));
	
	if(workZoneCheck) {
		line(draw1,_drawOrigin,p1,Scalar(0,0,255),4);
		line(draw1,p1,p2,Scalar(0,255,0),4);
		line(draw1,p2,p3,Scalar(255,0,0),4);
		circle(draw1,Point(_drawOrigin.x+_drawScale*_state.terminalXTh,_drawOrigin.y-_drawScale*_state.terminalY),10,Scalar(240,0,0),2);
		
		line(draw2,_drawOrigin,pp1,Scalar(0,255,255),4);
		line(draw2,pp1,pp2,Scalar(255,0,0),4);
		circle(draw2,Point(_drawOrigin.x+_drawScale*_state.terminalX,_drawOrigin.y-_drawScale*_state.terminalZ),10,Scalar(240,0,0),2);
		
		
	}
	else {
		putText(draw1,"! SORTIE DE LA ZONE DE TRAVAIL !",Point(0,200),0,1,Scalar(0,0,255),3,8,false);
	}

	ostringstream ossPos,oss1,oss2,oss3,ossTh0,ossTh3;
	ossPos << " (" << _state.terminalX << "," << _state.terminalY << "," << _state.terminalZ << ")";
	putText(draw1,ossPos.str(),Point(_drawOrigin.x+_drawScale*_state.terminalXTh,_drawOrigin.y-_drawScale*_state.terminalY),1,1,Scalar(255,0,0),1);
	oss1 << "Alpha1 = " << _state.alpha1*180/M_PI;
	putText(draw1,oss1.str(),Point(_drawWidth-220,20),1,1,Scalar(0,0,255),1);
	oss2 << "Alpha2 = " << _state.alpha2*180/M_PI;
	putText(draw1,oss2.str(),Point(_drawWidth-220,40),1,1,Scalar(0,255,0),1);
	oss3 << "Alpha3 = " << _state.alpha3*180/M_PI;
	putText(draw1,oss3.str(),Point(_drawWidth-220,60),1,1,Scalar(255,0,0),1);
	ossTh0 << "Theta0 = " << _state.theta0*180/M_PI;
	putText(draw2,ossTh0.str(),Point(_drawWidth-220,20),1,1,Scalar(0,255,255),1);
	ossTh3 << "Theta3 = " << _state.theta3*180/M_PI;
	putText(draw2,ossTh3.str(),Point(_drawWidth-220,40),1,1,Scalar(255,0,0),1);

	ostringstream ossInx,ossIny,ossInz;
	ossInx << "Input X : " << _state.terminalX;
	ossIny << "Input Y : " << _state.terminalY;
	ossInz << "Input Z : " << _state.terminalZ;
	putText(draw1,ossInx.str(),Point(10,10),1,1,Scalar(255,255,255),1);
	putText(draw1,ossIny.str(),Point(10,30),1,1,Scalar(255,255,255),1);
	putText(draw1,ossInz.str(),Point(10,50),1,1,Scalar(255,255,255),1);
}

bool BotController::initSerial() {
	if(_withBot) {
		if((_fd=serialOpen("/dev/ttyUSB0"/*0*/,9600))<0) return false;
		//if(wiringPiSetup()==-1) return false;
	}
	return true;
}

void BotController::sendInt(int v) {
	if(_withBot) serialPutchar(_fd,v);
}

void BotController::sendAngle(double angle) {
	int ang=(int) angle;
	cout << ang << ", ";
	sendInt(ang);
}

void BotController::sendToMotors() {
	cout << "## SENT " << string(_withBot?"(really)":"(virtually)") << " ";

	sendInt(250);	
	sendAngle(_state.theta0*180/M_PI*180/140);
	sendAngle(_state.alpha1*180/M_PI*180/140);
	sendAngle(_state.alpha2*180/M_PI);
	sendAngle(_state.alpha3*180/M_PI);
	sendAngle(_state.theta3*180/M_PI);

	cout << endl;

	delay(30);
}

void BotController::sendToVehicle() {
	sendInt(251);
	sendInt(128+_vehicleLeftSpeed);
	sendInt(128+_vehicleRightSpeed);
	delay(30);
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
	_state=computeAngles(_state);
	loopAngles();
}

void BotController::initStartUpRoutine() {
	BotState start;
	start.theta0=90;
	start.alpha1=180;
	start.alpha2=-170;
	start.alpha3=0;
	start.theta3=90;

	BotState end;
	end.theta0=90;
	end.alpha1=140;
	end.alpha2=-107;
	end.alpha3=0;
	end.theta3=0;
	end.alpha3=_terminalAbsAlpha-end.alpha1-end.alpha2;

	BotState current=start;

	_trajectory.clear();
	_trajectory.push_back(current);

	//Compute
	bool finished=false;
	double halfElbo=end.alpha2+((start.alpha2-end.alpha2)/2);

	while(!finished) {
		if((current.alpha2<halfElbo)) {
			current.alpha2++;
			_trajectory.push_back(current);
		}
		else if(current.theta3>end.theta3) {
			current.theta3-=3;
			_trajectory.push_back(current);
		}
		else if(current.alpha3>end.alpha3) {
			current.alpha3-=3;
			_trajectory.push_back(current);
		}
		else {
			if((current.alpha2<end.alpha2)) current.alpha2++;
			if((current.alpha1>end.alpha1)) current.alpha1--;
			_trajectory.push_back(current);
		}

		if(current.theta0==end.theta0 && current.alpha1==end.alpha1 && current.alpha2==end.alpha2 && current.theta3==end.theta3) finished=true;

	}

	_trajIt=_trajectory.begin();

}

bool BotController::loopStartUpRoutine() {
	_state.theta0=conv(0,true,(*_trajIt).theta0*M_PI/180);
	_state.alpha1=conv(1,true,(*_trajIt).alpha1*M_PI/180);
	_state.alpha2=conv(2,true,(*_trajIt).alpha2*M_PI/180);
	_state.alpha3=conv(3,true,(*_trajIt).alpha3*M_PI/180);
	_state.theta3=conv(4,true,(*_trajIt).theta3*M_PI/180);
	_state.length3Al=_length3*cos((*_trajIt).theta3*M_PI/180);
	_state.wristX=_length1*cos((*_trajIt).alpha1*M_PI/180)+_length2*cos((*_trajIt).alpha1*M_PI/180+((*_trajIt).alpha2*M_PI/180));
	_trajIt++;
	if(_trajIt==_trajectory.end()) return false;
	loopAngles();	
	Timer::wait(20);	
	//checkBatteryLevel();
	return true;
}

void BotController::initPreShutDownRoutine() {

	Position start,end;
	start.x=_state.terminalX;
	start.y=_state.terminalY;
	start.z=_state.terminalZ;
	end.x=0;
	end.y=35;
	end.z=22;
	genTrajectory(start,end);

}

void BotController::initShutDownRoutine() {

	_state.terminalX=0;
	_state.terminalY=35;
	_state.terminalZ=22;

	BotState start;
	start.theta0=90;
	start.alpha1=140;
	start.alpha2=-107;
	start.alpha3=0;
	start.theta3=0;

	BotState end;
	end.theta0=90;
	end.alpha1=180;
	end.alpha2=-170;
	end.alpha3=0;
	end.theta3=90;
	
	start.alpha3=_terminalAbsAlpha-start.alpha1-start.alpha2;

	BotState current=start;

	_trajectory.clear();
	_trajectory.push_back(current);

	//Compute
	bool finished=false;
	double halfElbo=start.alpha2+((end.alpha2-start.alpha2)/2);

	while(!finished) {
		if(current.alpha1<end.alpha1) {
			current.alpha1++;
			_trajectory.push_back(current);
		}
		else if((current.alpha2>halfElbo)) {
			current.alpha2--;
			_trajectory.push_back(current);
		}
		else if(current.theta3<end.theta3) {
			current.theta3+=3;
			_trajectory.push_back(current);
		}
		else if(current.alpha3<end.alpha3) {
			current.alpha3+=3;
			_trajectory.push_back(current);
		}
		else {
			if((current.alpha2>end.alpha2)) current.alpha2--;
			if((current.alpha1<end.alpha1)) current.alpha1++;
			_trajectory.push_back(current);
		}

		if(current.theta0==end.theta0 && current.alpha1==end.alpha1 && current.alpha2==end.alpha2 && current.theta3==end.theta3) finished=true;

	}

	_trajIt=_trajectory.begin();

}

bool BotController::loopPreShutDownRoutine() {
	if(_trajIt==_trajectory.end()) return false;
	else {
		copyState(_state,*_trajIt);
		_state=computeAngles(_state);
		loopAngles();
		_trajIt++;
		return true;
	}
}

bool BotController::loopShutDownRoutine() {
	_state.theta0=conv(0,true,(*_trajIt).theta0*M_PI/180);
	_state.alpha1=conv(1,true,(*_trajIt).alpha1*M_PI/180);
	_state.alpha2=conv(2,true,(*_trajIt).alpha2*M_PI/180);
	_state.alpha3=conv(3,true,(*_trajIt).alpha3*M_PI/180);
	_state.theta3=conv(4,true,(*_trajIt).theta3*M_PI/180);
	_state.length3Al=_length3*cos((*_trajIt).theta3*M_PI/180);
	_state.wristX=_length1*cos((*_trajIt).alpha1*M_PI/180)+_length2*cos((*_trajIt).alpha1*M_PI/180+((*_trajIt).alpha2*M_PI/180));
	loopAngles();	
	Timer::wait(20);
	_trajIt++;
	if(_trajIt==_trajectory.end()) return false;	
	//checkBatteryLevel();
	return true;
}

void BotController::genTrajectory(Position a,Position b,double step) {
	BotState stA,stB;
	stA.terminalX=a.x;
	stA.terminalY=a.y;
	stA.terminalZ=a.z;
	stB.terminalX=b.x;
	stB.terminalY=b.y;
	stB.terminalZ=b.z;

	BotState vect;
	vect.terminalX=b.x-a.x;
	vect.terminalY=b.y-a.y;
	vect.terminalZ=b.z-a.z;

	double norm=sqrt(vect.terminalX*vect.terminalX+vect.terminalY*vect.terminalY+vect.terminalZ*vect.terminalZ);
	vect.terminalX=vect.terminalX/norm*step;
	vect.terminalY=vect.terminalY/norm*step;
	vect.terminalZ=vect.terminalZ/norm*step;

	_trajectory.clear();
	_trajectory.push_back(stA);

	for(double i=0;i<norm;i+=step) {
		stA.terminalX+=vect.terminalX;
		stA.terminalY+=vect.terminalY;
		stA.terminalZ+=vect.terminalZ;
		_trajectory.push_back(stA);
	}

	_trajectory.push_back(stB);
	_trajIt=_trajectory.begin();
}

void BotController::checkBatteryLevel() {
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

double BotController::conv(unsigned int servo,bool unit,double input) {
	//servo = 0:_theta0, 1:_alpha1, 2:_alpha2, 3:_alpha3, 4:_theta3
	//unit : true=computed to real ; false=real to computed
	
	//COMPUTED
	//	theta0 : 0 complètement à droite, 180 à gauche
	//	theta3 : 0 dans le prolongement, 90 à gauche (angle droit)
	//	alpha1 : 0 à l'horizontale devant, 90 en haut
	//	alpha2 : 0 dans le prolongement, 90 angle droit vers le haut, -90 angle droit vers le bas
	//	alpha3 : 0 dans le prolongement, 90 angle droit vers le haut, -90 angle droit vers le bas
	//REAL
	//	theta0 : 0-140, 0 à droite (pas complètement), 70 en face (à multiplier par 180/140 pour envoyer au servo)
	//	theta3 : 90 dans le prolongement, (0 ??)
	//	alpha1 : 0-140, 140 à l'horizontale arrière (à multiplier par 180/140 pour envoyer au servo)
	//	alpha2 : 0-180, 0 replié par le bas sur la première pièce, 180 dans le prolongement
	//	alpha3 : 0-180, 90 dans le prolongement, (0 ??)

	if(unit) {
		if(servo==0) return input-20*M_PI/180;
		else if(servo==4) return input+90*M_PI/180; //+90° si 0 à droite, 90°-_theta3 si 0 à gauche
		else if(servo==1) return input-40*M_PI/180;
		else if(servo==2) return input+180*M_PI/180;
		else return input+90*M_PI/180; //+90° si 0 en bas, 90°-_alpha3 si 0 en haut
	}
	else {
		if(servo==0) return input+20*M_PI/180;
		else if(servo==4) return input-90*M_PI/180; //-90° si 0 à droite, 90°-_theta3 si 0 à gauche
		else if(servo==1) return input+40*M_PI/180;
		else if(servo==2) return input-180*M_PI/180;
		else return input-90*M_PI/180; //-90° si 0 en bas, 90°-_alpha3 si 0 en haut
	}
}


