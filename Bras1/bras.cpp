#include <opencv2/opencv.hpp>
#include <cmath>
#include <sstream>

using namespace cv;
using namespace std;

const double terminalAbsAngle=20*M_PI/180;
const double length1=30;
const double length2=30;
const double length3=10;

const double drawScale=6.5;
const unsigned int drawWidth=640;
const unsigned int drawHeight=480;

double theta1,theta2,theta3;
int terminalX=30,terminalY=30;

Point drawOrigin(200,425);

void drawAxis(Mat& draw);
void drawBot(Mat& draw);

int main() {
	namedWindow("Bras",1);
	
	while(true) {
		
		Mat draw(drawHeight,drawWidth,CV_8UC3);
		createTrackbar("X = ","Bras",&terminalX,70);
		createTrackbar("Y = ","Bras",&terminalY,70);
		
		double wristX=terminalX-length3*cos(terminalAbsAngle);
		double wristY=terminalY-length3*sin(terminalAbsAngle);
		
		//cf 379.pdf :
		double cosTheta2=(wristX*wristX+wristY*wristY-length1*length1-length2*length2)/(2*length1*length2);
		double sinTheta2=-sqrt(1-cosTheta2*cosTheta2);
		double cosTheta1=(wristX*(length1+length2*cosTheta2)+wristY*length2*sinTheta2)/(wristX*wristX+wristY*wristY);
		double sinTheta1=sqrt(1-cosTheta1*cosTheta1);
		theta1=atan2(sinTheta1,cosTheta1);
		theta2=atan2(sinTheta2,cosTheta2);
		theta3=terminalAbsAngle-theta1-theta2;
		
		//Draw axis
		drawAxis(draw);
		
		//Draw bot
		drawBot(draw);
		
		imshow("Bras",draw);
		if(waitKey(30)>=0) break;
		
	}
	
	return 0;
}

void drawAxis(Mat& draw) {
	line(draw,Point(drawOrigin.x,0),Point(drawOrigin.x,drawHeight),Scalar(255,255,255),1);
	line(draw,Point(0,drawOrigin.y),Point(drawWidth,drawOrigin.y),Scalar(255,255,255),1);
	putText(draw,"y",Point(drawOrigin.x+10,20),0,1,Scalar(255,255,255),1);
	putText(draw,"x",Point(drawWidth-20,drawOrigin.y-10),0,1,Scalar(255,255,255),1);
	putText(draw,"Simulation bras robotique",Point(10,drawHeight-20),2,1,Scalar(0,255,255),2);
}

void drawBot(Mat& draw) {
	Point p1(drawOrigin.x+drawScale*length1*cos(theta1),drawOrigin.y-drawScale*length1*sin(theta1));
	Point p2(p1.x+drawScale*length1*cos(theta2+theta1),p1.y-drawScale*length1*sin(theta2+theta1));
	Point p3(p2.x+drawScale*length3*cos(theta1+theta2+theta3),p2.y-drawScale*length3*sin(theta1+theta2+theta3));
	if(sqrt(terminalX*terminalX+terminalY*terminalY)<=length1+length2+length3) {
		line(draw,drawOrigin,p1,Scalar(0,0,255),4);
		line(draw,p1,p2,Scalar(0,255,0),4);
		line(draw,p2,p3,Scalar(255,0,0),4);
		circle(draw,Point(drawOrigin.x+drawScale*terminalX,drawOrigin.y-drawScale*terminalY),10,Scalar(240,0,0),2);
		
		ostringstream ossPos,oss1,oss2,oss3;
		ossPos << " (" << terminalX << "," << terminalY << ")";
		putText(draw,ossPos.str(),Point(drawOrigin.x+drawScale*terminalX,drawOrigin.y-drawScale*terminalY),1,1,Scalar(255,0,0),1);
		oss1 << "Theta1 = " << theta1*180/M_PI;
		putText(draw,oss1.str(),Point(drawWidth-220,20),1,1,Scalar(0,0,255),1);
		oss2 << "Theta2 = " << theta2*180/M_PI;
		putText(draw,oss2.str(),Point(drawWidth-220,40),1,1,Scalar(0,255,0),1);
		oss3 << "Theta3 = " << theta3*180/M_PI;
		putText(draw,oss3.str(),Point(drawWidth-220,60),1,1,Scalar(255,0,0),1);
	}
	else {
		putText(draw,"! SORTIE DE LA ZONE DE TRAVAIL !",Point(0,50),0,1,Scalar(0,0,255),3,8,false);
	}
}

