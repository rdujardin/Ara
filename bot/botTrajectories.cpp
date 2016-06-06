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
#define __computed(a,x) conv(a,false,x)*180/M_PI
#define __real(a,x) conv(a,true,x)*M_PI/180

void BotTrajectories::initStartUpRoutine(Trajectory& _trajectory,TrajIt& _trajIt,BotState& _state) {
	BotState start,end;
	#define c(a,x) conv(a,true,x*M_PI/180)
	start.setUnsafe(angles,theta0,c(0,90),alpha1,c(1,180),alpha2,c(2,-170),alpha3,c(3,0),theta3,c(4,90));
	end.setUnsafe(angles,theta0,c(0,90),alpha1,c(1,140),alpha2,c(2,-107),alpha3,c(3,0),theta3,c(4,0));

	double v_al1=conv(1,false,end.get(alpha1));
	double v_al2=conv(2,false,end.get(alpha2));
	end.setUnsafe(angles,alpha3,conv(3,true,_terminalAbsAlpha-v_al1-v_al2));

	BotState current=start;

	_trajectory.clear();
	_trajectory.push_back(current);

	//Compute
	bool finished=false;
	//double halfElbo=end.get(alpha2)+((start.get(alpha2)-end.get(alpha2))/2);
	double halfElbo=__computed(2,end.get(alpha2))+(__computed(2,start.get(alpha2))-__computed(2,end.get(alpha2)))/2;

	while(!finished) {
		if (waitKey(1)>=0) break;
		//if((current.get(alpha2)<halfElbo)) {
		if(__computed(2,current.get(alpha2))<halfElbo) {
			double v=conv(2,false,current.get(alpha2))*180/M_PI;
			v+=1;
			current.setUnsafe(angles,alpha2,conv(2,true,v*M_PI/180));
			_trajectory.push_back(current);
		}
		//else if(current.get(theta3)>end.get(theta3)) {
		else if(__computed(4,current.get(theta3))>__computed(4,end.get(theta3))) {
			double v=conv(4,false,current.get(theta3))*180/M_PI;
			v-=3;
			current.setUnsafe(angles,theta3,conv(4,true,v*M_PI/180));
			_trajectory.push_back(current);
		}
		//else if(current.get(alpha3)>end.get(alpha3)) {
		else if(__computed(3,current.get(alpha3))>__computed(3,end.get(alpha3))) {
			double v=conv(3,false,current.get(alpha3))*180/M_PI;
			v-=3;
			current.setUnsafe(angles,alpha3,conv(3,true,v*M_PI/180));
			_trajectory.push_back(current);
		}
		else {
			//if((current.get(alpha2)<end.get(alpha2))) {
			if(__computed(2,current.get(alpha2))<__computed(2,end.get(alpha2))) {
				double v=conv(2,false,current.get(alpha2))*180/M_PI;
				v+=1;
				current.setUnsafe(angles,alpha2,conv(2,true,v*M_PI/180));
			}
			//if((current.get(alpha1)>end.get(alpha1))) {
			if(__computed(1,current.get(alpha1))>__computed(1,end.get(alpha1))) {
				double v=conv(1,false,current.get(alpha1))*180/M_PI;
				v-=1;
				current.setUnsafe(angles,alpha1,conv(1,true,v*M_PI/180));
			}
			_trajectory.push_back(current);
		}

		if(near(current.get(theta0),end.get(theta0)) && near(current.get(alpha1),end.get(alpha1)) && near(current.get(alpha2),end.get(alpha2)) && near(current.get(theta3),end.get(theta3))) finished=true;

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
	#define c(a,x) conv(a,true,x*M_PI/180)
	start.setUnsafe(angles,theta0,c(0,90),alpha1,c(1,140),alpha2,c(2,-107),alpha3,c(3,0),theta3,c(4,0));
	end.setUnsafe(angles,theta0,c(0,90),alpha1,c(1,180),alpha2,c(2,-170),alpha3,c(3,0),theta3,c(4,90));

	double v_al1=conv(1,false,start.get(alpha1));
	double v_al2=conv(2,false,start.get(alpha2));
	start.setUnsafe(angles,alpha3,conv(3,true,_terminalAbsAlpha-v_al1-v_al2));

	BotState current=start;

	_trajectory.clear();
	_trajectory.push_back(current);

	//Compute
	bool finished=false;
	//double halfElbo=start.get(alpha2)+((end.get(alpha2)-start.get(alpha2))/2);
	double halfElbo=__computed(2,start.get(alpha2))+((__computed(2,end.get(alpha2))-__computed(2,start.get(alpha2)))/2);

	while(!finished) {
		if (waitKey(1)>=0) break;
		//if(current.get(alpha1)<end.get(alpha1)) {
		if(__computed(1,current.get(alpha1))<__computed(1,end.get(alpha1))) {
			double v=conv(1,false,current.get(alpha1))*180/M_PI;
			v+=1;
			current.setUnsafe(angles,alpha1,conv(1,true,v*M_PI/180));
			_trajectory.push_back(current);
		}
		//else if((current.get(alpha2)>halfElbo)) {
		else if(__computed(2,current.get(alpha2))>halfElbo) {
			double v=conv(2,false,current.get(alpha2))*180/M_PI;
			v-=1;
			current.setUnsafe(angles,alpha2,conv(2,true,v*M_PI/180));
			_trajectory.push_back(current);
		}
		//else if(current.get(theta3)<end.get(theta3)) {
		else if(__computed(4,current.get(theta3))<__computed(4,end.get(theta3))) {
			double v=conv(4,false,current.get(theta3))*180/M_PI;
			v+=3;
			current.setUnsafe(angles,theta3,conv(4,true,v*M_PI/180));
			_trajectory.push_back(current);
		}
		//else if(current.get(alpha3)<end.get(alpha3)) {
		else if(__computed(3,current.get(alpha3))<__computed(3,end.get(alpha3))) {
			double v=conv(3,false,current.get(alpha3))*180/M_PI;
			v+=3;
			current.setUnsafe(angles,alpha3,conv(3,true,v*M_PI/180));
			_trajectory.push_back(current);
		}
		else {
			//if((current.get(alpha2)>end.get(alpha2))) {
			if(__computed(2,current.get(alpha2))>__computed(2,end.get(alpha2))) {
				double v=conv(2,false,current.get(alpha2))*180/M_PI;
				v-=1;
				current.setUnsafe(angles,alpha2,conv(2,true,v*M_PI/180));
			}
			//if((current.get(alpha1)<end.get(alpha1))) {
			if(__computed(1,current.get(alpha1))<__computed(1,end.get(alpha1))) {
				double v=conv(1,false,current.get(alpha1))*180/M_PI;
				v+=1;
				current.setUnsafe(angles,alpha1,conv(1,true,v*M_PI/180));
			}
			_trajectory.push_back(current);
		}

		if(near(current.get(theta0),end.get(theta0)) && near(current.get(alpha1),end.get(alpha1)) && near(current.get(alpha2),end.get(alpha2)) && near(current.get(theta3),end.get(theta3))) finished=true;

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
