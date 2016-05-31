/*
common.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#define COMMON_C
#include "common.h"

using namespace std;

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

double conv(unsigned int servo,bool unit,double input) {
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
