/*
botDraw.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "botDraw.h"

using namespace cv;
using namespace std;

const Point BotDraw::_drawOrigin=Point(200,425);

void BotDraw::drawAxis(Mat& draw1,Mat& draw2) {
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

	/*ostringstream oss;
	oss << "BATTERY ";
	if(_batteryLevel>=0) oss << _batteryLevel << "%";
	else if(_batteryLevel==-1) oss << "n.c.";
	else oss << "TOO LOW";
	putText(draw1,oss.str(),Point(_drawWidth-128,_drawHeight-25),1,1,Scalar(0,255,255),1);*/
}

void BotDraw::drawBot(Mat& draw1,Mat& draw2,BotState _state,bool workZoneCheck) {
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
