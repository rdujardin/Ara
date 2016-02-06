#include <opencv2/opencv.hpp>
#include <cmath>
#include <sstream>

using namespace cv;
using namespace std;

const double terminalAbsAlpha=20*M_PI/180;
const double terminalAbsTheta=0.01*M_PI/180;
const double length1=30;
const double length2=30;
const double length3=10;

const double drawScale=6.5;
const unsigned int drawWidth=640;
const unsigned int drawHeight=480;

double alpha1,alpha2,alpha3,theta0,theta3;
int terminalX=30,terminalY=30,terminalZ=30;

double wristX,wristY,terminalXTh,length3Al,length3Th=length3*cos(terminalAbsAlpha);

Point drawOrigin(200,425);

void drawAxis(Mat& draw1,Mat& draw2);
void drawBot(Mat& draw1,Mat& draw2);

int main() {
	namedWindow("Bras (vue de côté)",1);
	namedWindow("Bras (vue de haut)",1);
	
	moveWindow("Bras (vue de côté)",0,200);
	moveWindow("Bras (vue de haut)",640,300);
	
	while(true) {
		
		Mat draw1(drawHeight,drawWidth,CV_8UC3);
		Mat draw2(drawHeight,drawWidth,CV_8UC3);
		createTrackbar("X = ","Bras (vue de côté)",&terminalX,70);
		createTrackbar("Y = ","Bras (vue de côté)",&terminalY,70);
		createTrackbar("Z = ","Bras (vue de côté)",&terminalZ,70);

		//calcul theta :
		theta0=atan2((terminalZ-length3*cos(terminalAbsAlpha)*cos(terminalAbsTheta)),(terminalX+length3*cos(terminalAbsAlpha)*sin(terminalAbsTheta)));
		theta3=M_PI/2-terminalAbsTheta-theta0;
		length3Al=length3*cos(theta3);
		wristX=(terminalX+length3*cos(terminalAbsAlpha)*sin(terminalAbsTheta))/cos(theta0);
		//wristX=terminalX-length3*cos(terminalAbsAngle);
		wristY=terminalY-length3*sin(terminalAbsAlpha);
		terminalXTh=wristX+length3Al*cos(terminalAbsAlpha);
		
		//cf 379.pdf :
		double cosAlpha2=(wristX*wristX+wristY*wristY-length1*length1-length2*length2)/(2*length1*length2);
		double sinAlpha2=-sqrt(1-cosAlpha2*cosAlpha2);
		double cosAlpha1=(wristX*(length1+length2*cosAlpha2)+wristY*length2*sinAlpha2)/(wristX*wristX+wristY*wristY);
		double sinAlpha1=sqrt(1-cosAlpha1*cosAlpha1);
		alpha1=atan2(sinAlpha1,cosAlpha1);
		alpha2=atan2(sinAlpha2,cosAlpha2);
		alpha3=terminalAbsAlpha-alpha1-alpha2;
		
		//Draw axis
		drawAxis(draw1,draw2);
		
		//Draw bot
		drawBot(draw1,draw2);
		
		imshow("Bras (vue de côté)",draw1);
		imshow("Bras (vue de haut)",draw2);
		if(waitKey(30)>=0) break;
		
	}
	
	return 0;
}

void drawAxis(Mat& draw1,Mat& draw2) {
	line(draw1,Point(drawOrigin.x,0),Point(drawOrigin.x,drawHeight),Scalar(255,255,255),1);
	line(draw1,Point(0,drawOrigin.y),Point(drawWidth,drawOrigin.y),Scalar(255,255,255),1);
	putText(draw1,"y",Point(drawOrigin.x+10,20),0,1,Scalar(255,255,255),1);
	putText(draw1,"xTh",Point(drawWidth-20,drawOrigin.y-10),0,1,Scalar(255,255,255),1);
	putText(draw1,"Vue de côté",Point(10,drawHeight-20),2,1,Scalar(0,255,255),2);
	
	line(draw2,Point(drawOrigin.x,0),Point(drawOrigin.x,drawHeight),Scalar(255,255,255),1);
	line(draw2,Point(0,drawOrigin.y),Point(drawWidth,drawOrigin.y),Scalar(255,255,255),1);
	putText(draw2,"z",Point(drawOrigin.x+10,20),0,1,Scalar(255,255,255),1);
	putText(draw2,"x",Point(drawWidth-20,drawOrigin.y-10),0,1,Scalar(255,255,255),1);
	putText(draw2,"Vue de dessus",Point(10,drawHeight-20),2,1,Scalar(0,255,255),2);
}

void drawBot(Mat& draw1,Mat& draw2) {
	Point p1(drawOrigin.x+drawScale*length1*cos(alpha1),drawOrigin.y-drawScale*length1*sin(alpha1));
	Point p2(p1.x+drawScale*length2*cos(alpha2+alpha1),p1.y-drawScale*length2*sin(alpha2+alpha1));
	Point p3(p2.x+drawScale*length3Al*cos(alpha1+alpha2+alpha3),p2.y-drawScale*length3Al*sin(alpha1+alpha2+alpha3));
	
	Point pp1(drawOrigin.x+drawScale*wristX*cos(theta0),drawOrigin.y-drawScale*wristX*sin(theta0));
	Point pp2(pp1.x+drawScale*length3Th*cos(theta0+theta3),pp1.y-drawScale*length3Th*sin(theta0+theta3));
	
	if(sqrt(terminalXTh*terminalXTh+terminalY*terminalY)<=length1+length2+length3 && sqrt(terminalX*terminalX+terminalZ*terminalZ)<=length1+length2+length3) {
		line(draw1,drawOrigin,p1,Scalar(0,0,255),4);
		line(draw1,p1,p2,Scalar(0,255,0),4);
		line(draw1,p2,p3,Scalar(255,0,0),4);
		circle(draw1,Point(drawOrigin.x+drawScale*terminalXTh,drawOrigin.y-drawScale*terminalY),10,Scalar(240,0,0),2);
		
		line(draw2,drawOrigin,pp1,Scalar(0,255,255),4);
		line(draw2,pp1,pp2,Scalar(255,0,0),4);
		circle(draw2,Point(drawOrigin.x+drawScale*terminalX,drawOrigin.y-drawScale*terminalZ),10,Scalar(240,0,0),2);
		
		ostringstream ossPos,oss1,oss2,oss3,ossTh0,ossTh3;
		ossPos << " (" << terminalX << "," << terminalY << "," << terminalZ << ")";
		putText(draw1,ossPos.str(),Point(drawOrigin.x+drawScale*terminalXTh,drawOrigin.y-drawScale*terminalY),1,1,Scalar(255,0,0),1);
		oss1 << "Alpha1 = " << alpha1*180/M_PI;
		putText(draw1,oss1.str(),Point(drawWidth-220,20),1,1,Scalar(0,0,255),1);
		oss2 << "Alpha2 = " << alpha2*180/M_PI;
		putText(draw1,oss2.str(),Point(drawWidth-220,40),1,1,Scalar(0,255,0),1);
		oss3 << "Alpha3 = " << alpha3*180/M_PI;
		putText(draw1,oss3.str(),Point(drawWidth-220,60),1,1,Scalar(255,0,0),1);
		ossTh0 << "Theta0 = " << theta0*180/M_PI;
		putText(draw2,ossTh0.str(),Point(drawWidth-220,20),1,1,Scalar(0,255,255),1);
		ossTh3 << "Theta3 = " << theta3*180/M_PI;
		putText(draw2,ossTh3.str(),Point(drawWidth-220,40),1,1,Scalar(255,0,0),1);
	}
	else {
		putText(draw1,"! SORTIE DE LA ZONE DE TRAVAIL !",Point(0,50),0,1,Scalar(0,0,255),3,8,false);
	}
}

