#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <sstream>
#include <ctime>

#include "timer.h"
#include "hsv.h"
#include "filters.h"
#include "hough.h"
#include "camera.h"

using namespace std;
using namespace cv;

enum State {
	PLACE_BALL=0,
	RUNNING=1
};

int main(int argc,char* argv[]) {

	State state=(argc>1)?PLACE_BALL:RUNNING;

	cv::namedWindow("Input", 1);
	cv::namedWindow("Output", 2);
	cv::namedWindow("Settings", 3);
	cv::namedWindow("Hsv", 4);
	cv::namedWindow("Camera Settings", 5);
	
	moveWindow("Input",520,40);
	moveWindow("Hsv",520,308);
	moveWindow("Output",520,576);
	moveWindow("Camera Settings",840,40);
	moveWindow("Settings",200,40);
	
	Camera cam(true);
	if (!cam.isOpened()) return -1;

	vector<Vec3f> circles;

	Timer timer;
	HSV_Thresholder hsvThresholder(timer, true);
	GaussianFilter gaussianFilter(timer, true);
	DilateEroder dilateEroder(timer, true);
	Hough hough(timer,circles);
	
	//createTrackbar(trackbarName,winName,int*,int max);

	vector< vector<Point> > contours;
	
	while (true) {

		timer.reset();

		Mat input, output;
		cam >> input;
		
		Mat resized;
		resize(input,resized,Size(320,240));
		
		if(state==PLACE_BALL) {
			circle(resized,Point(160,120),HSV_Thresholder::autoSetRadius,Scalar(0,0,255),4);
			Mat flipped;
			flip(resized,flipped,1);
			
			imshow("Input",flipped);
			imshow("Hsv",flipped);
			imshow("Output",flipped);
			
			if(waitKey(30)>=0) {
				hsvThresholder.autoSet(input);
				state=RUNNING;
			}
		}
		
		else {
		
			gaussianFilter(resized);
			
			imshow("Input", resized);

			hsvThresholder(resized, output);
		
			imshow("Hsv", output);
		
			//Mat filtered(output.rows, output.cols,output.type());
			//bilateralFilter(output, filtered, 5, 150, 150);
			dilateEroder(output);
			//hough(output, input);

			//filtered.copyTo(output);
		
			/*findContours(output, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
			Mat ct = Mat::zeros(output.size(), CV_8UC1);
			for (int i = 0; i < contours.size(); i++) {
				drawContours(ct, contours, i, Scalar(255, 255, 255), 1);
			}*/
		
			hough(output,resized);
		
			unsigned int size=256;
			//Mat test(size,size,CV_8UC1);
			Mat test=Mat::zeros(size,size,CV_8UC1);
			circle(test,Point(128,128),128,255,-1);
		
			imshow("Input", resized);
			imshow("Output", output);

			cout << "(" << input.rows << "," << input.cols << ")-(" << output.rows << "," << output.cols << ") # ";

			string sep = " ms # ";
			cout << "TOTAL " << timer.total() << sep;
			cout << "Hsv " << hsvThresholder.time() << sep;
			cout << "Gauss " << gaussianFilter.time() << sep;
			cout << "Dil&Er " << dilateEroder.time() << sep;
			cout << "Hough " << hough.time() << sep;
			cout << endl;

			if (waitKey(30) >= 0) break;
		}
	}


	return 0;
}

