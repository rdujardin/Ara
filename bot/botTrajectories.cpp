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

using namespace std;
using namespace cv;

void BotTrajectories::genTrajectory(Position a,Position b,Trajectory& _trajectory,TrajIt& _trajIt,double step) {
	BotState stA,stB;
	stA.set(cartesian,terminalX,a.x,terminalY,a.y,terminalZ,a.z);
	stB.set(cartesian,terminalX,b.x,terminalY,b.y,terminalZ,b.z);

	_trajectory.clear();
	_trajectory.push_back(stA);

	if(!(a.x==b.x && a.y==b.y && a.z==b.z)) {
		BotState vect;
		vect.setUnsafe(cartesian,terminalX,b.x-a.x,terminalY,b.y-a.y,terminalZ,b.z-a.z);
		double norm=sqrt(vect.get(terminalX)*vect.get(terminalX)+vect.get(terminalY)*vect.get(terminalY)+vect.get(terminalZ)*vect.get(terminalZ));
		vect.setUnsafe(cartesian,terminalX,vect.get(terminalX)/norm*step,terminalY,vect.get(terminalY)/norm*step,terminalZ,vect.get(terminalZ)/norm*step);

		_trajectory.clear();
		_trajectory.push_back(stA);

		for(double i=0;i<norm;i+=step) {
			stA.set(cartesian,terminalX,stA.get(terminalX)+vect.get(terminalX),terminalY,stA.get(terminalY)+vect.get(terminalY),terminalZ,stA.get(terminalZ)+vect.get(terminalZ));
			_trajectory.push_back(stA);
		}
	}

	_trajectory.push_back(stB);
	_trajIt=_trajectory.begin();
}

#define __deg *180/M_PI
#define __rad *M_PI/180
#define rad(x) x*M_PI/180
#define deg(x) x*180/M_PI
#define __computed(a,x) conv(a,false,x)*180/M_PI
#define __real(a,x) conv(a,true,x)*M_PI/180

#define THETA0_SPEED 1
#define ALPHA1_SPEED 1
#define ALPHA2_SPEED 1
#define ALPHA3_SPEED 2
#define THETA3_SPEED 2

void BotTrajectories::initStartUpRoutine(Trajectory& _trajectory,TrajIt& _trajIt,BotState& _state) {
	BotState start,end;
	start.setUnsafe(angles,theta0,rad(ALPHA1_2_RANGE/2),alpha1,rad(ALPHA1_2_RANGE),alpha2,rad(0),alpha3,rad(90),theta3,rad(180));
	end.setUnsafe(angles,theta0,rad(ALPHA1_2_RANGE/2),alpha1,rad(87),alpha2,rad(70),alpha3,rad(13),theta3,rad(90));

	BotState current;
	current.setFrom(start);
	bool finished=false;

	while(!finished) {
		if(waitKey(1)>=0) break;

		if(current.get(alpha2)<end.get(alpha2) && !nearAng1(current,end,alpha2,rad(ALPHA2_SPEED)))
			current.setUnsafe(angles,alpha2,current.get(alpha2)+rad(ALPHA2_SPEED));
		else if(current.get(alpha1)>end.get(alpha1) && !nearAng1(current,end,alpha1,rad(ALPHA1_SPEED)))
			current.setUnsafe(angles,alpha1,current.get(alpha1)-rad(ALPHA1_SPEED));
		else if(current.get(theta3)>end.get(theta3) && !nearAng1(current,end,theta3,rad(THETA3_SPEED)))
			current.setUnsafe(angles,theta3,current.get(theta3)-rad(THETA3_SPEED));
		else if(current.get(alpha3)>end.get(alpha3) && !nearAng1(current,end,alpha3,rad(ALPHA3_SPEED)))
			current.setUnsafe(angles,alpha3,current.get(alpha3)-rad(ALPHA3_SPEED));

		_trajectory.push_back(current);
		cout << current.stringify() << endl;
		if(nearAng1(current,end,theta0,rad(THETA0_SPEED)) && nearAng1(current,end,alpha1,rad(ALPHA1_SPEED)) && nearAng1(current,end,alpha2,rad(ALPHA2_SPEED)) && nearAng1(current,end,alpha3,rad(ALPHA3_SPEED)) && nearAng1(current,end,theta3,rad(THETA3_SPEED))) finished=true;
	}

	_trajIt=_trajectory.begin();

}

void BotTrajectories::initPreShutDownRoutine(Trajectory& _trajectory,TrajIt& _trajIt,BotState& _state) {

	Position start,end;
	start.x=_state.get(terminalX);
	start.y=_state.get(terminalY);
	start.z=_state.get(terminalZ);
	end.x=0;
	end.y=/*35*/15;
	end.z=22;
	genTrajectory(start,end,_trajectory,_trajIt);

}

void BotTrajectories::initShutDownRoutine(Trajectory& _trajectory,TrajIt& _trajIt,BotState& _state) {

	_state.set(cartesian,terminalX,0,terminalY,/*35*/15,terminalZ,22);
	BotState start,end;
	end.setUnsafe(angles,theta0,rad(ALPHA1_2_RANGE/2),alpha1,rad(ALPHA1_2_RANGE),alpha2,rad(0),alpha3,rad(90),theta3,rad(180));
	start.setUnsafe(angles,theta0,rad(ALPHA1_2_RANGE/2),alpha1,rad(87),alpha2,rad(70),alpha3,rad(13),theta3,rad(90));

	BotState current;
	current.setFrom(start);
	bool finished=false;

	while(!finished) {
		if(waitKey(1)>=0) break;

		if(current.get(theta3)<end.get(theta3) && !nearAng1(current,end,theta3,rad(THETA3_SPEED)))
			current.setUnsafe(angles,theta3,current.get(theta3)+rad(THETA3_SPEED));
		else if(current.get(alpha3)<end.get(alpha3) && !nearAng1(current,end,alpha3,rad(ALPHA3_SPEED)))
			current.setUnsafe(angles,alpha3,current.get(alpha3)+rad(ALPHA3_SPEED));
		else if(current.get(alpha1)<end.get(alpha1) && !nearAng1(current,end,alpha1,rad(ALPHA1_SPEED)))
			current.setUnsafe(angles,alpha1,current.get(alpha1)+rad(ALPHA1_SPEED));
		else if(current.get(alpha2)>end.get(alpha2) && !nearAng1(current,end,alpha2,rad(ALPHA2_SPEED)))
			current.setUnsafe(angles,alpha2,current.get(alpha2)-rad(ALPHA2_SPEED));
		
		

		_trajectory.push_back(current);
		cout << current.stringify() << endl;
		if(nearAng1(current,end,theta0,rad(THETA0_SPEED)) && nearAng1(current,end,alpha1,rad(ALPHA1_SPEED)) && nearAng1(current,end,alpha2,rad(ALPHA2_SPEED)) && nearAng1(current,end,alpha3,rad(ALPHA3_SPEED)) && nearAng1(current,end,theta3,rad(THETA3_SPEED))) finished=true;
	}

	_trajIt=_trajectory.begin();

	
}

bool BotTrajectories::loopStartUpRoutine(BotState& _state,Trajectory& _trajectory,TrajIt& _trajIt) {
	if(_trajIt==_trajectory.end()) return false;
	_state.setFrom(*_trajIt);
	return true;
}

bool BotTrajectories::loopPreShutDownRoutine(BotState& _state,Trajectory& _trajectory,TrajIt& _trajIt) {
	if(_trajIt==_trajectory.end()) return false;
	else {
		_state.setFrom(*_trajIt);
		return true;
	}
}

bool BotTrajectories::loopShutDownRoutine(BotState& _state,Trajectory& _trajectory,TrajIt& _trajIt) {
	if(_trajIt==_trajectory.end()) return false;
	_state.setFrom(*_trajIt);
	return true;
}
