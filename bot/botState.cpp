/*
bot_vals[cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "botState.h"

using namespace std;

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
	//	theta0 : 0-140, 0 à droite (pas complètement), 70 en face (à multiplier par 180/140 pour envoyer au servo) //en fait c'est 0 à gauche ! 
	//	theta3 : 90 dans le prolongement, (0 ??)
	//	alpha1 : 0-140, 140 à l'horizontale arrière (à multiplier par 180/140 pour envoyer au servo)
	//	alpha2 : 0-180, 0 replié par le bas sur la première pièce, 180 dans le prolongement
	//	alpha3 : 0-180, 90 dans le prolongement, (0 ??)

	if(unit) {
		if(servo==0) return (ALPHA1_2_RANGE-(input*180/M_PI-(180-ALPHA1_2_RANGE)/2))*M_PI/180;
		else if(servo==4) return input+90*M_PI/180; //+90° si 0 à droite, 90°-_theta3 si 0 à gauche
		else if(servo==1) return input-(180-ALPHA1_2_RANGE)*M_PI/180;
		else if(servo==2) return input+180*M_PI/180;
		else return input+90*M_PI/180; //+90° si 0 en bas, 90°-_alpha3 si 0 en haut
	}
	else {
		if(servo==0) return (180-(input*180/M_PI+(180-ALPHA1_2_RANGE)/2))*M_PI/180;
		else if(servo==4) return input-90*M_PI/180; //-90° si 0 à droite, 90°-_theta3 si 0 à gauche
		else if(servo==1) return input+(180-ALPHA1_2_RANGE)*M_PI/180;
		else if(servo==2) return input-180*M_PI/180;
		else return input-90*M_PI/180; //-90° si 0 en bas, 90°-_alpha3 si 0 en haut
	}
}

BotState::BotState() {
	for(unsigned int i=0;i<=12;++i) _vals.push_back(0);
}

BotState BotState::readyPos() {
	BotState s;
	s.set(angles,theta0,rad(ALPHA1_2_RANGE/2.0),alpha1,rad(87.0),alpha2,rad(70.0),alpha3,rad(13.0),theta3,rad(90.0));
	return s;
}

BotState BotState::offPos() {
	BotState s;
	s.setUnsafe(angles,theta0,rad(ALPHA1_2_RANGE/2.0),alpha1,rad(ALPHA1_2_RANGE),alpha2,rad(0.0),alpha3,rad(90.0),theta3,rad(180.0));
	return s;
}
/*
void BotState::setFrom(BotState& s) {
	_vals.clear();
	for(unsigned int i=0;i<=12;++i) _vals.push_back(s._vals[i]);
}
*/
void BotState::setFrom(BotState s) {
	_vals.clear();
	for(unsigned int i=0;i<=12;++i) _vals.push_back(s._vals[i]);
}

double BotState::get(unsigned int i) {
	return _vals[i];
}

bool BotState::set(unsigned int type,unsigned int i1,double v1,unsigned int i2,double v2,unsigned i3,double v3,unsigned i4,double v4,unsigned i5,double v5,unsigned i6,double v6,unsigned i7,double v7,unsigned i8,double v8,unsigned i9,double v9,unsigned i10,double v10,unsigned i11,double v11) {
	vector<double> sv=_vals;

	setUnsafe(type,i1,v1,i2,v2,i3,v3,i4,v4,i5,v5,i6,v6,i7,v7,i8,v8,i9,v9,i10,v10,i11,v11);

	if(!checkWorkingZone()) {
		_vals=sv;
		return false;
	}
	else return true;
}

void BotState::setUnsafe(unsigned int type,unsigned int i1,double v1,unsigned int i2,double v2,unsigned i3,double v3,unsigned i4,double v4,unsigned i5,double v5,unsigned i6,double v6,unsigned i7,double v7,unsigned i8,double v8,unsigned i9,double v9,unsigned i10,double v10,unsigned i11,double v11) {
	map<unsigned int,double> v;
	v[0]=0;
	v[i1]=v1;
	v[i2]=v2;
	v[i3]=v3;
	v[i4]=v4;
	v[i5]=v5;
	v[i6]=v6;
	v[i7]=v7;
	v[i8]=v8;
	v[i9]=v9;
	v[i10]=v10;
	v[i11]=v11;

	for(map<unsigned int,double>::iterator it=v.begin();it!=v.end();++it) {
		_vals[it->first]=it->second;
	}

	if(type==angles) computeCartesian();
	else computeAngles();
}

bool BotState::check(unsigned int i) {
	if(i==theta0) return _vals[theta0]>=0*M_PI/180 && _vals[theta0]<=125*M_PI/180;
	else if(i==alpha1) return _vals[alpha1]>=0*M_PI/180 && _vals[alpha1]<=125*M_PI/180;
	else if(i==alpha2) return _vals[alpha2]>=0*M_PI/180 && _vals[alpha2]<=180*M_PI/180;
	else if(i==alpha3) return _vals[alpha3]>=0*M_PI/180 && _vals[alpha3]<=180*M_PI/180;
	else if(i==theta3) return _vals[theta3]>=0*M_PI/180 && _vals[theta3]<=180*M_PI/180;
	else if(i==terminalY) return _vals[terminalY]>0+_terminalYOffset;
	else if(i==terminalZ) return _vals[terminalZ]>=20 && _vals[terminalZ]<55;
	else if(i==wristY) return _vals[wristY]>0;
	else return true;
}

bool BotState::checkWorkingZone() {
	bool w=true;
	for(unsigned int i=0;i<=12;++i) w=w && check(i);
	return w;
}

void BotState::computeAngles() {
	//Angles computing :
	_vals[theta0]=atan2((_vals[terminalZ]-_length3*cos(_terminalAbsAlpha)*cos(_terminalAbsTheta)),(_vals[terminalX]+_length3*cos(_terminalAbsAlpha)*sin(_terminalAbsTheta)));
	_vals[theta3]=M_PI/2-_terminalAbsTheta-_vals[theta0];
	_vals[length3Al]=_length3*cos(_vals[theta3]);
	_vals[wristX]=(_vals[terminalX]+_length3*cos(_terminalAbsAlpha)*sin(_terminalAbsTheta))/cos(_vals[theta0]);
	_vals[wristY]=_vals[terminalY]-_length3*sin(_terminalAbsAlpha);
	_vals[terminalXTh]=_vals[wristX]+_vals[length3Al]*cos(_terminalAbsAlpha);

	//cf 379.pdf :
	double cosAlpha2=(_vals[wristX]*_vals[wristX]+_vals[wristY]*_vals[wristY]-_length1*_length1-_length2*_length2)/(2*_length1*_length2);
	double sinAlpha2=-sqrt(1-cosAlpha2*cosAlpha2);
	double cosAlpha1=(_vals[wristX]*(_length1+_length2*cosAlpha2)+_vals[wristY]*_length2*sinAlpha2)/(_vals[wristX]*_vals[wristX]+_vals[wristY]*_vals[wristY]);
	double sinAlpha1=sqrt(1-cosAlpha1*cosAlpha1);
	_vals[alpha1]=atan2(sinAlpha1,cosAlpha1);
	_vals[alpha2]=atan2(sinAlpha2,cosAlpha2);
	_vals[alpha3]=_terminalAbsAlpha-_vals[alpha1]-_vals[alpha2];

	//Conversion : computed->real
	_vals[theta0]=conv(0,true,_vals[theta0]);
	_vals[alpha1]=conv(1,true,_vals[alpha1]);
	_vals[alpha2]=conv(2,true,_vals[alpha2]);
	_vals[alpha3]=conv(3,true,_vals[alpha3]);
	_vals[theta3]=conv(4,true,_vals[theta3]);
}

void BotState::computeCartesian() {
	_vals[length3Al]=_length3*cos(conv(4,false,_vals[theta3]));
	_vals[wristX]=_length1*cos(conv(1,false,_vals[alpha1]))+_length2*cos(conv(1,false,_vals[alpha1])+conv(2,false,_vals[alpha2]));
	_vals[wristY]=_length1*sin(conv(1,false,_vals[alpha1]))+_length2*sin(conv(1,false,_vals[alpha1])+conv(2,false,_vals[alpha2]));
	_vals[terminalXTh]=_vals[wristX]+_vals[length3Al]*cos(_terminalAbsAlpha);
	_vals[terminalX]=_vals[wristX]*cos(conv(0,false,_vals[theta0]))+_length3Th*cos(conv(0,false,_vals[theta0])+conv(4,false,_vals[theta3]));
	_vals[terminalY]=_length1*sin(conv(1,false,_vals[alpha1]))+_length2*sin(conv(1,false,_vals[alpha1])+conv(2,false,_vals[alpha2]))+_vals[length3Al]*sin(conv(1,false,_vals[alpha1])+conv(2,false,_vals[alpha2])+conv(3,false,_vals[alpha3]));
	_vals[terminalZ]=_vals[wristX]*sin(conv(0,false,_vals[theta0]))+_length3Th*sin(conv(0,false,_vals[theta0])+conv(4,false,_vals[theta3]));
}

string BotState::stringify() {
	ostringstream oss;
	oss << "[x " << _vals[terminalX] << " | y " << _vals[terminalY] << " | z " << _vals[terminalZ] << " | th0 " << _vals[theta0] << " | al1 " << _vals[alpha1] << " | al2 " << _vals[alpha2] << " | al3 " << _vals[alpha3] << " | th3 " << _vals[theta3] << "]";
	return oss.str();
}
