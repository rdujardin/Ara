#include <cstring>
#include <iostream>

#include "common.h"
#include "ballDetector.h"
#include "botController.h"

using namespace std;
using namespace cv;

int main(int argc,char* argv[]) {
	bool withBot=(argc>1);

	Camera* cam=new Camera(true);
	if (!cam->isOpened()) throw -1;

	namedWindow("Settings", 3);
	moveWindow("Settings",200,40);

	/*cv::namedWindow("Camera Settings", 5);
	moveWindow("Camera Settings",840,40);*/
	
	namedWindow("Input", 1);
	namedWindow("Output", 1);
	namedWindow("Hsv", 1);
	namedWindow("Trajectory",1);
	moveWindow("Input",0,0);
	moveWindow("Hsv",700,0);
	moveWindow("Trajectory",1200,0);

	namedWindow("Bras (vue de cote)",1);
	namedWindow("Bras (vue de haut)",1);

	//moveWindow("Bras (vue de cote)",0,300);
	//moveWindow("Bras (vue de haut)",640,300);
	moveWindow("Bras (vue de cote)",0,600);
	moveWindow("Bras (vue de haut)",740,600);

	/*Mat empty=Mat::zeros(Camera::width,Camera::height,CV_8UC3);
	imshow("Input",empty);
	imshow("Output",empty);
	imshow("Hsv",empty);
	imshow("Trajectory",empty);
	imshow("Bras (vue de cote)",empty);
	imshow("Bras (vue de haut)",empty);*/
	
	activatedWindows["Settings"]=true;
	activatedWindows["Camera Settings"]=false;
	activatedWindows["Bras (vue de cote)"]=true;

	BallDetector* ballDetector=NULL;
	ballDetector=new BallDetector(cam);
	BotController* botController=new BotController(withBot);

	while(true) {
		Position pos;
		ballDetector->loop(pos);
		//cout << "####POSITION(cam) " << pos.x << " / " << pos.y << " / " << pos.z << endl;
		double horizAngle=0; //°, angle de la caméra % à l'horizontale, orienté vers le haut     <<<<<<<<<<<< ANGLE HORIZONTALE CAMERA HERE
		//Horizon rotation :
		double tmpY=pos.y;
		horizAngle=-horizAngle*M_PI/180;
		pos.y+=30;
		//pos.y=cos(horizAngle)*tmpY+sin(horizAngle)*pos.z;
		//pos.z=-sin(horizAngle)*tmpY+cos(horizAngle)*pos.z;
		//cout << "####POSITION(bot) " << pos.x << " / " << pos.y << " / " << pos.z << endl;
		if(!botController->loop(pos)) break;
		
		char key=waitKey(1);
		if(key==27) botController->nextState();
	}

	delete botController;
	delete ballDetector;
	delete cam;

	return 0;
}

