#include "botController.h"

using namespace cv;
using namespace std;

const Point BotController::_drawOrigin=Point(200,425);
const string BotController::_strCliQuit=string("quit");

BotController::BotController(Mode mode,bool withBot,bool withGui,bool adjustable) : Adjustable("Bras (vue de cote)") {
	_mode=mode;	
	_withBot=withBot;
	_withGui=withGui;
	_adjustable=adjustable;
	
	_terminalX=0;
	_terminalY=35;
	_terminalZ=22;

	if(_withBot)
		if(!initSerial()) throw -1;

	if(_withGui) {
		namedWindow("Bras (vue de cote)",1);
		namedWindow("Bras (vue de haut)",1);
	
		//moveWindow("Bras (vue de cote)",0,300);
		//moveWindow("Bras (vue de haut)",640,300);
		moveWindow("Bras (vue de cote)",0,600);
		moveWindow("Bras (vue de haut)",740,600);
		
		_params["X = "]=_terminalX*2+70;
		_params["Y = "]=_terminalY;
		_params["Z = "]=_terminalZ*2+40;
		
		if(_adjustable) {
			makeAdjustable("X = ",140);
			makeAdjustable("Y = ",70);
			makeAdjustable("Z = ",140);
		}
	}

	_batteryLevel=-1;
	receiveVoltage();

	if(_mode!=ONLY_DETECTION) startUpRoutine();
}

BotController::~BotController() {
	if(_mode!=ONLY_DETECTION) shutDownRoutine();
}

bool BotController::loop(Position detection) {

	_terminalX=detection.x;
	_terminalY=detection.y;
	_terminalZ=detection.z;
	
	//Angles computing :
	_theta0=atan2((_terminalZ-_length3*cos(_terminalAbsAlpha)*cos(_terminalAbsTheta)),(_terminalX+_length3*cos(_terminalAbsAlpha)*sin(_terminalAbsTheta)));
	_theta3=M_PI/2-_terminalAbsTheta-_theta0;
	_length3Al=_length3*cos(_theta3);
	_wristX=(_terminalX+_length3*cos(_terminalAbsAlpha)*sin(_terminalAbsTheta))/cos(_theta0);
	_wristY=_terminalY-_length3*sin(_terminalAbsAlpha);
	_terminalXTh=_wristX+_length3Al*cos(_terminalAbsAlpha);
	
	//cf 379.pdf :
	double cosAlpha2=(_wristX*_wristX+_wristY*_wristY-_length1*_length1-_length2*_length2)/(2*_length1*_length2);
	double sinAlpha2=-sqrt(1-cosAlpha2*cosAlpha2);
	double cosAlpha1=(_wristX*(_length1+_length2*cosAlpha2)+_wristY*_length2*sinAlpha2)/(_wristX*_wristX+_wristY*_wristY);
	double sinAlpha1=sqrt(1-cosAlpha1*cosAlpha1);
	_alpha1=atan2(sinAlpha1,cosAlpha1);
	_alpha2=atan2(sinAlpha2,cosAlpha2);
	_alpha3=_terminalAbsAlpha-_alpha1-_alpha2;

	//Conversion : computed->real
	_theta0=conv(0,true,_theta0);
	_alpha1=conv(1,true,_alpha1);
	_alpha2=conv(2,true,_alpha2);
	_alpha3=conv(3,true,_alpha3);
	_theta3=conv(4,true,_theta3);

	return loopAngles();
	
}

bool BotController::checkWorkingZone() {
	bool w=true;
	w=w && _theta0>=0*M_PI/180 && _theta0<=140*M_PI/180;
	w=w && _alpha1>=0*M_PI/180 && _alpha1<=140*M_PI/180;
	w=w && _alpha2>=0*M_PI/180 && _alpha2<=180*M_PI/180;
	w=w && _alpha3>=0*M_PI/180 && _alpha3<=180*M_PI/180;
	w=w && _theta3>=0*M_PI/180 && _theta3<=180*M_PI/180;

	w=w && _terminalY>0;
	w=w && _terminalZ>=20;
	w=w && _terminalZ<45;

	return w;
}

bool BotController::loopAngles() {
	Mat draw1=Mat::zeros(_drawHeight,_drawWidth,CV_8UC3);
	Mat draw2=Mat::zeros(_drawHeight,_drawWidth,CV_8UC3);

	//VÉRIFICATION DE LA ZONE DE TRAVAIL
	bool workZoneCheck=checkWorkingZone();

	if(_withGui) {
		//Draw axis
		drawAxis(draw1,draw2);
		
		//Draw bot
		drawBot(draw1,draw2,workZoneCheck);
		
		imshow("Bras (vue de cote)",draw1);
		imshow("Bras (vue de haut)",draw2);
	}

	//double angle_test(__alpha0);
	if(workZoneCheck) sendToMotors();
	
	if(_adjustable) {
		if (waitKey(30) >= 0) return false;
	}

	receiveVoltage();
	
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
	else oss << "n.c.";
	putText(draw1,oss.str(),Point(_drawWidth-128,_drawHeight-25),1,1,Scalar(0,255,255),1);
}

void BotController::drawBot(Mat& draw1,Mat& draw2,bool workZoneCheck) {
	double alpha1=conv(1,false,_alpha1);
	double alpha2=conv(2,false,_alpha2);
	double alpha3=conv(3,false,_alpha3);
	double theta0=conv(0,false,_theta0);
	double theta3=conv(4,false,_theta3);

	Point p1(_drawOrigin.x+_drawScale*_length1*cos(alpha1),_drawOrigin.y-_drawScale*_length1*sin(alpha1));
	Point p2(p1.x+_drawScale*_length2*cos(alpha2+alpha1),p1.y-_drawScale*_length2*sin(alpha2+alpha1));
	Point p3(p2.x+_drawScale*_length3Al*cos(alpha1+alpha2+alpha3),p2.y-_drawScale*_length3Al*sin(alpha1+alpha2+alpha3));
	
	Point pp1(_drawOrigin.x+_drawScale*_wristX*cos(theta0),_drawOrigin.y-_drawScale*_wristX*sin(theta0));
	Point pp2(pp1.x+_drawScale*_length3Th*cos(theta0+theta3),pp1.y-_drawScale*_length3Th*sin(theta0+theta3));
	
	if(workZoneCheck) {
		line(draw1,_drawOrigin,p1,Scalar(0,0,255),4);
		line(draw1,p1,p2,Scalar(0,255,0),4);
		line(draw1,p2,p3,Scalar(255,0,0),4);
		circle(draw1,Point(_drawOrigin.x+_drawScale*_terminalXTh,_drawOrigin.y-_drawScale*_terminalY),10,Scalar(240,0,0),2);
		
		line(draw2,_drawOrigin,pp1,Scalar(0,255,255),4);
		line(draw2,pp1,pp2,Scalar(255,0,0),4);
		circle(draw2,Point(_drawOrigin.x+_drawScale*_terminalX,_drawOrigin.y-_drawScale*_terminalZ),10,Scalar(240,0,0),2);
		
		
	}
	else {
		putText(draw1,"! SORTIE DE LA ZONE DE TRAVAIL !",Point(0,50),0,1,Scalar(0,0,255),3,8,false);
	}

	ostringstream ossPos,oss1,oss2,oss3,ossTh0,ossTh3;
	ossPos << " (" << _terminalX << "," << _terminalY << "," << _terminalZ << ")";
	putText(draw1,ossPos.str(),Point(_drawOrigin.x+_drawScale*_terminalXTh,_drawOrigin.y-_drawScale*_terminalY),1,1,Scalar(255,0,0),1);
	oss1 << "Alpha1 = " << _alpha1*180/M_PI;
	putText(draw1,oss1.str(),Point(_drawWidth-220,20),1,1,Scalar(0,0,255),1);
	oss2 << "Alpha2 = " << _alpha2*180/M_PI;
	putText(draw1,oss2.str(),Point(_drawWidth-220,40),1,1,Scalar(0,255,0),1);
	oss3 << "Alpha3 = " << _alpha3*180/M_PI;
	putText(draw1,oss3.str(),Point(_drawWidth-220,60),1,1,Scalar(255,0,0),1);
	ossTh0 << "Theta0 = " << _theta0*180/M_PI;
	putText(draw2,ossTh0.str(),Point(_drawWidth-220,20),1,1,Scalar(0,255,255),1);
	ossTh3 << "Theta3 = " << _theta3*180/M_PI;
	putText(draw2,ossTh3.str(),Point(_drawWidth-220,40),1,1,Scalar(255,0,0),1);

	ostringstream ossInx,ossIny,ossInz;
	ossInx << "Input X : " << _terminalX;
	ossIny << "Input Y : " << _terminalY;
	ossInz << "Input Z : " << _terminalZ;
	putText(draw1,ossInx.str(),Point(10,10),1,1,Scalar(255,255,255),1);
	putText(draw1,ossIny.str(),Point(10,30),1,1,Scalar(255,255,255),1);
	putText(draw1,ossInz.str(),Point(10,50),1,1,Scalar(255,255,255),1);
}

bool BotController::initSerial() {
	if(_withBot) {
		if((_fd=serialOpen("/dev/ttyUSB0",9600))<0) return false;
		//if(wiringPiSetup()==-1) return false;
	}
	return true;
}

void BotController::sendInt(int v) {
	if(_withBot) serialPutchar(_fd,v);
	cout << v << " , ";
}

int BotController::safe(int v) {
	if(v<0) return 0;
	else if(v>180) return 180;
	else return v;
}

void BotController::sendAngle(double angle) {
	//sendInt(safe((int) (angle))); //retrait de safe
	sendInt((int) angle);
}

void BotController::sendToMotors() {
	cout << "## SENT " << string(_withBot?"(really)":"(virtually)") << " ";

	/*double alpha1=conv(1,true,_alpha1);
	double alpha2=conv(2,true,_alpha2);
	double alpha3=conv(3,true,_alpha3);
	double theta0=conv(0,true,_theta0);
	double theta3=conv(4,true,_theta3);*/

	sendInt(250);	
	//double baseAngle=0.78*(180-_theta0*180/M_PI);
	//baseAngle=0.78*theta0;
	sendAngle(_theta0*180/M_PI); //Contraintes du a la position du servomoteur+conversion 180degrés vers 140degrés

	//double shouAngle=0.78*(_alpha1*180/M_PI);	
	sendAngle(_alpha1*180/M_PI); //avant : 180-le tout

	/*sendAngle(0.78*(180-_alpha2*180/M_PI));
	sendAngle(90+_alpha3*180/M_PI);
	sendAngle(90+_theta3*180/M_PI);	
	*/
	sendAngle(_alpha2*180/M_PI);
	sendAngle(_alpha3*180/M_PI);
	sendAngle(_theta3*180/M_PI);
	cout << endl;


	delay(30);

	if(_withBot) {
		char ret[100];
		int i=0;
		while (serialDataAvail (_fd))
		{
	  		ret[i]= serialGetchar (_fd) ;
			i++;
		}
		ret[i]=0;
		if(strlen(ret)>0) cout << "-> Arduino says : " << endl << ret;
	}
	
	cout << endl;
	
}

void BotController::adjusted(std::string name,int val) {
	_params[name]=val;
	_terminalX=(_params["X = "]-70)/2;
	_terminalY=_params["Y = "];
	_terminalZ=(_params["Z = "]-40)/2;
}

void BotController::startUpRoutine() {
	//routine(true);
	//BotState start(90,180,-170,160,90);
	BotState start;
	start.push_back(90);
	start.push_back(180);
	start.push_back(-170);
	start.push_back(0); //160
	start.push_back(90);
	//BotState end(90,100,-90,160,0);
	BotState end;
	end.push_back(90); //90
	end.push_back(140); //145
	end.push_back(-107); //-123
	end.push_back(0); //-1.7
	end.push_back(0); //0
	end[3]=_terminalAbsAlpha-end[1]-end[2];

	BotState current=start;

	vector<BotState> trajectory;
	trajectory.push_back(current);

	//Calculate
	bool finished=false;
	double halfElbo=end[2]+((start[2]-end[2])/2);

	while(!finished) {
		if((current[2]<halfElbo)) {
			current[2]++;
			trajectory.push_back(current);
		}
		else if(current[4]>end[4]) {
			current[4]-=3;
			trajectory.push_back(current);
		}
		else if(current[3]>end[3]) {
			current[3]-=3;
			trajectory.push_back(current);
		}
		else {
			if((current[2]<end[2])) current[2]++;
			if((current[1]>end[1])) current[1]--;
			trajectory.push_back(current);
		}

		if(current[0]==end[0] && current[1]==end[1] && current[2]==end[2] && current[4]==end[4]) finished=true;

	}

	//Execute
	for(vector<BotState>::iterator it=trajectory.begin();it!=trajectory.end();++it) {
		_theta0=conv(0,true,(*it)[0]*M_PI/180);
		_alpha1=conv(1,true,(*it)[1]*M_PI/180);
		_alpha2=conv(2,true,(*it)[2]*M_PI/180);
		_alpha3=conv(3,true,(*it)[3]*M_PI/180);
		_theta3=conv(4,true,(*it)[4]*M_PI/180);
		_length3Al=_length3*cos((*it)[4]*M_PI/180);
		_wristX=_length1*cos((*it)[1]*M_PI/180)+_length2*cos((*it)[1]*M_PI/180+((*it)[2]*M_PI/180));
		loopAngles();
		Timer::wait(20);
	}
}

void BotController::shutDownRoutine() {
	_terminalX=0;
	_terminalY=35;
	_terminalZ=22;	

	BotState start;
	

	start.push_back(90);
	start.push_back(140);
	start.push_back(-107);
	start.push_back(0);
	start.push_back(0);
	//BotState end(90,100,-90,160,0);
	BotState end;
	end.push_back(90);
	end.push_back(180);
	end.push_back(-170);
	end.push_back(0);
	end.push_back(90);
	
	start[3]=_terminalAbsAlpha-start[1]-start[2];

	BotState current=start;

	vector<BotState> trajectory;
	trajectory.push_back(current);

	//Calculate
	bool finished=false;
	double halfElbo=start[2]+((end[2]-start[2])/2);

	while(!finished) {
		if(current[1]<end[1]) {
			current[1]++;
			trajectory.push_back(current);
		}
		else if((current[2]>halfElbo)) {
			current[2]--;
			trajectory.push_back(current);
		}
		else if(current[4]<end[4]) {
			current[4]+=3;
			trajectory.push_back(current);
		}
		else if(current[3]<end[3]) {
			current[3]+=3;
			trajectory.push_back(current);
		}
		else {
			if((current[2]>end[2])) current[2]--;
			if((current[1]<end[1])) current[1]++;
			trajectory.push_back(current);
		}

		if(current[0]==end[0] && current[1]==end[1] && current[2]==end[2] && current[4]==end[4]) finished=true;

	}

	//Execute
	for(vector<BotState>::iterator it=trajectory.begin();it!=trajectory.end();++it) {
		_theta0=conv(0,true,(*it)[0]*M_PI/180);
		_alpha1=conv(1,true,(*it)[1]*M_PI/180);
		_alpha2=conv(2,true,(*it)[2]*M_PI/180);
		_alpha3=conv(3,true,(*it)[3]*M_PI/180);
		_theta3=conv(4,true,(*it)[4]*M_PI/180);
		_length3Al=_length3*cos((*it)[4]*M_PI/180);
		_wristX=_length1*cos((*it)[1]*M_PI/180)+_length2*cos((*it)[1]*M_PI/180+((*it)[2]*M_PI/180));
		loopAngles();
		Timer::wait(20);
	}
}

void BotController::routine(bool startup) {
	//.
}

void BotController::receiveVoltage() {
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
		if(strlen(ret)>0) _batteryLevel=ret[0];
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


