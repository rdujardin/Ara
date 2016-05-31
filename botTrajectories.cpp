/*
botTrajectories.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "botTrajectories.h"

void BotTrajectories::genTrajectory(Position a,Position b,Trajectory& _trajectory,TrajIt& _trajIt,double step) {
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

void BotTrajectories::initStartUpRoutine(Trajectory& _trajectory,TrajIt& _trajIt,BotState& _state) {
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

void BotTrajectories::initPreShutDownRoutine(Trajectory& _trajectory,TrajIt& _trajIt,BotState& _state) {

	Position start,end;
	start.x=_state.terminalX;
	start.y=_state.terminalY;
	start.z=_state.terminalZ;
	end.x=0;
	end.y=35;
	end.z=22;
	genTrajectory(start,end,_trajectory,_trajIt);

}

void BotTrajectories::initShutDownRoutine(Trajectory& _trajectory,TrajIt& _trajIt,BotState& _state) {

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

bool BotTrajectories::loopStartUpRoutine(BotState& _state,Trajectory& _trajectory,TrajIt& _trajIt) {
	_state.theta0=conv(0,true,(*_trajIt).theta0*M_PI/180);
	_state.alpha1=conv(1,true,(*_trajIt).alpha1*M_PI/180);
	_state.alpha2=conv(2,true,(*_trajIt).alpha2*M_PI/180);
	_state.alpha3=conv(3,true,(*_trajIt).alpha3*M_PI/180);
	_state.theta3=conv(4,true,(*_trajIt).theta3*M_PI/180);
	_state.length3Al=_length3*cos((*_trajIt).theta3*M_PI/180);
	_state.wristX=_length1*cos((*_trajIt).alpha1*M_PI/180)+_length2*cos((*_trajIt).alpha1*M_PI/180+((*_trajIt).alpha2*M_PI/180));
	_state.wristY=99;
	if(_trajIt==_trajectory.end()) return false;
	return true;
}

bool BotTrajectories::loopPreShutDownRoutine(BotState& _state,Trajectory& _trajectory,TrajIt& _trajIt) {
	if(_trajIt==_trajectory.end()) return false;
	else {
		copyState(_state,*_trajIt);
		_state=computeAngles(_state);
		return true;
	}
}

bool BotTrajectories::loopShutDownRoutine(BotState& _state,Trajectory& _trajectory,TrajIt& _trajIt) {
	_state.theta0=conv(0,true,(*_trajIt).theta0*M_PI/180);
	_state.alpha1=conv(1,true,(*_trajIt).alpha1*M_PI/180);
	_state.alpha2=conv(2,true,(*_trajIt).alpha2*M_PI/180);
	_state.alpha3=conv(3,true,(*_trajIt).alpha3*M_PI/180);
	_state.theta3=conv(4,true,(*_trajIt).theta3*M_PI/180);
	_state.length3Al=_length3*cos((*_trajIt).theta3*M_PI/180);
	_state.wristX=_length1*cos((*_trajIt).alpha1*M_PI/180)+_length2*cos((*_trajIt).alpha1*M_PI/180+((*_trajIt).alpha2*M_PI/180));
	_state.wristY=99;
	if(_trajIt==_trajectory.end()) return false;
	return true;
}

BotState BotTrajectories::computeAngles(BotState state) {
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
