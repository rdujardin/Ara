#include "botController.h"

using namespace cv;
using namespace std;

const Point BotController::_drawOrigin=Point(200,425);
const string BotController::_strCliQuit=string("quit");

BotController::BotController(bool withBot,bool withGui,bool adjustable) : Adjustable("Bras (vue de cote)") {
	_withBot=withBot;
	_withGui=withGui;
	_adjustable=adjustable;
	
	_terminalX=30;
	_terminalY=30;
	_terminalZ=20;

	if(_withBot)
		if(!initSerial()) throw -1;

	if(_withGui) {
		namedWindow("Bras (vue de cote)",1);
		namedWindow("Bras (vue de haut)",1);
	
		moveWindow("Bras (vue de cote)",0,300);
		moveWindow("Bras (vue de haut)",640,300);
		
		_params["X = "]=_terminalX;
		_params["Y = "]=_terminalY;
		_params["Z = "]=_terminalZ;
		
		if(_adjustable) {
			makeAdjustable("X = ",70);
			makeAdjustable("Y = ",70);
			makeAdjustable("Z = ",70);
		}
	}
}

bool BotController::loop(Position detection) {

	Mat draw1(_drawHeight,_drawWidth,CV_8UC3);
	Mat draw2(_drawHeight,_drawWidth,CV_8UC3);

	if(!_withGui && _adjustable) {
		cout << "X ? ";
		cin >> _inputX;
		if(_inputX==_strCliQuit) return false;
		cout << "Y ? ";
		cin >> _inputY;
		if(_inputY==_strCliQuit) return false;
		cout << "Z ? ";
		cin >> _inputZ;
		if(_inputZ==_strCliQuit) return false;
		_terminalX=atoi(_inputX.c_str());
		_terminalY=atoi(_inputY.c_str());
		_terminalZ=atoi(_inputZ.c_str());
	}
	
	if(!_adjustable) {
		_terminalX=(int) detection.x;
		_terminalY=(int) detection.y;
		_terminalZ=(int) detection.z;
	}
	
	cout << "X : " << _terminalX << " / Y : " << _terminalY << " / Z : " << _terminalZ << endl;

	//calcul theta :
	_theta0=atan2((_terminalZ-_length3*cos(_terminalAbsAlpha)*cos(_terminalAbsTheta)),(_terminalX+_length3*cos(_terminalAbsAlpha)*sin(_terminalAbsTheta)));
	_theta3=M_PI/2-_terminalAbsTheta-_theta0;
	_length3Al=_length3*cos(_theta3);
	_wristX=(_terminalX+_length3*cos(_terminalAbsAlpha)*sin(_terminalAbsTheta))/cos(_theta0);
	//wristX=terminalX-length3*cos(terminalAbsAngle);
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
	
	if(_withGui) {
		//Draw axis
		drawAxis(draw1,draw2);
		
		//Draw bot
		drawBot(draw1,draw2);
		
		imshow("Bras (vue de cote)",draw1);
		imshow("Bras (vue de haut)",draw2);
		//if(waitKey(30)>=0) return false;
	}
	else {
		cout << "alpha1 = " << _alpha1*180/M_PI << endl;
		cout << "alpha2 = " << _alpha2*180/M_PI << endl;
		cout << "alpha3 = " << _alpha3*180/M_PI << endl;
		cout << "theta0 = " << _theta0*180/M_PI << endl;
		cout << "theta3 = " << _theta3*180/M_PI << endl;
		cout << "-------------------------" << endl;
	}

	//double angle_test(__alpha0);
	if(sqrt(_terminalXTh*_terminalXTh+_terminalY*_terminalY)<=_length1+_length2+_length3 && sqrt(_terminalX*_terminalX+_terminalZ*_terminalZ)<=_length1+_length2+_length3) sendToMotors();
	
	if(_adjustable) {
		if (waitKey(30) >= 0) return false;
	}
	
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
}

void BotController::drawBot(Mat& draw1,Mat& draw2) {
	Point p1(_drawOrigin.x+_drawScale*_length1*cos(_alpha1),_drawOrigin.y-_drawScale*_length1*sin(_alpha1));
	Point p2(p1.x+_drawScale*_length2*cos(_alpha2+_alpha1),p1.y-_drawScale*_length2*sin(_alpha2+_alpha1));
	Point p3(p2.x+_drawScale*_length3Al*cos(_alpha1+_alpha2+_alpha3),p2.y-_drawScale*_length3Al*sin(_alpha1+_alpha2+_alpha3));
	
	Point pp1(_drawOrigin.x+_drawScale*_wristX*cos(_theta0),_drawOrigin.y-_drawScale*_wristX*sin(_theta0));
	Point pp2(pp1.x+_drawScale*_length3Th*cos(_theta0+_theta3),pp1.y-_drawScale*_length3Th*sin(_theta0+_theta3));
	
	if(sqrt(_terminalXTh*_terminalXTh+_terminalY*_terminalY)<=_length1+_length2+_length3 && sqrt(_terminalX*_terminalX+_terminalZ*_terminalZ)<=_length1+_length2+_length3) {
		line(draw1,_drawOrigin,p1,Scalar(0,0,255),4);
		line(draw1,p1,p2,Scalar(0,255,0),4);
		line(draw1,p2,p3,Scalar(255,0,0),4);
		circle(draw1,Point(_drawOrigin.x+_drawScale*_terminalXTh,_drawOrigin.y-_drawScale*_terminalY),10,Scalar(240,0,0),2);
		
		line(draw2,_drawOrigin,pp1,Scalar(0,255,255),4);
		line(draw2,pp1,pp2,Scalar(255,0,0),4);
		circle(draw2,Point(_drawOrigin.x+_drawScale*_terminalX,_drawOrigin.y-_drawScale*_terminalZ),10,Scalar(240,0,0),2);
		
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
	}
	else {
		putText(draw1,"! SORTIE DE LA ZONE DE TRAVAIL !",Point(0,50),0,1,Scalar(0,0,255),3,8,false);
	}
}

bool BotController::initSerial() {
	if(_withBot) {
		if((_fd=serialOpen("/dev/ttyUSB0",9600))<0) return false;
		if(wiringPiSetup()==-1) return false;
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
	sendInt(safe((int) (angle))); //retrait de safe
}

void BotController::sendToMotors() {
	cout << "## SENT " << string(_withBot?"(really)":"(virtually)") << " ";

	sendInt(250);	
	sendAngle(0.78*(180-_theta0*180/M_PI)); //Contraintes du a la position du servomoteur+conversion 180degrés vers 140degrés
	sendAngle(0.78*(_alpha1*180/M_PI)); //avant : 180-le tout
	sendAngle(0.78*(-_alpha2*180/M_PI));
	sendAngle(90+_alpha3*180/M_PI);
	sendAngle(90+_theta3*180/M_PI);	
	
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
	_terminalX=_params["X = "];
	_terminalY=_params["Y = "];
	_terminalZ=_params["Z = "];
}

