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

using namespace std;
using namespace cv;

int main() {
	VideoCapture cam(CV_CAP_ANY);
	if (!cam.isOpened()) return -1;
	
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, 960);
	cam.set(CV_CAP_PROP_FPS, 25);

	cv::namedWindow("Input", 1);
	cv::namedWindow("Output", 2);
	cv::namedWindow("Settings", 3);

	vector<Vec3f> circles;

	Timer timer;
	HSV_Thresholder hsvThresholder(timer, false);
	GaussianFilter gaussianFilter(timer);
	DilateEroder dilateEroder(timer, true);
	Hough hough(timer,circles);
	
	//createTrackbar(trackbarName,winName,int*,int max);

	vector< vector<Point> > contours;
	
	while (true) {

		timer.reset();

		Mat input, output;
		cam >> input;
		
		resize(input,input,Size(320,240));

		hsvThresholder(input, output);
		gaussianFilter(output);
		//Mat filtered(output.rows, output.cols,output.type());
		//bilateralFilter(output, filtered, 5, 150, 150);
		dilateEroder(output);
		//hough(output, input);

		//filtered.copyTo(output);
		
		findContours(output, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		Mat ct = Mat::zeros(output.size(), CV_8UC1);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(ct, contours, i, Scalar(255, 255, 255), 2);
		}
		
		hough(ct,input);
		
		unsigned int size=256;
		Mat test(size,size,CV_8UC1);
		circle(test,Point(128,128),128,255,-1);
		
		imshow("Input", input);
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


	return 0;
}

