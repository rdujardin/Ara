#include "hough.h"

using namespace std;
using namespace cv;

Hough::Hough(Timer& timer, vector<Vec3f>& circles) : Timable(timer), _circles(circles) {}

void Hough::apply(Mat& src, Mat& dst) {
	timerStart();
	HoughCircles(src, _circles, CV_HOUGH_GRADIENT, 1, src.cols, 100, 15); //100,22
	timerStop();
	draw(dst);
}

void Hough::operator()(Mat& src, Mat& dst) {
	apply(src, dst);
}

void Hough::draw(Mat& dst) {
	for (size_t i = 0; i < _circles.size(); i++) {
		Point center(round(_circles[i][0]), round(_circles[i][1]));
		int radius = round(_circles[i][2]);
		circle(dst, center, 3, Scalar(0, 255, 0), 2);
		circle(dst, center, radius, Scalar(0, 0, 255), 3);
	}
}

