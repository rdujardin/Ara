#ifndef HOUGH_H
#define HOUGH_H

#include <opencv2/opencv.hpp>
#include <vector>

#include "timer.h"

class Hough : public Timable {
public:
	Hough(Timer& timer, std::vector<cv::Vec3f>& circles);

	void apply(cv::Mat& src, cv::Mat& dst);

	void operator()(cv::Mat& src, cv::Mat& dst);
private:
	std::vector<cv::Vec3f>& _circles;

	void draw(cv::Mat& dst);
};

#endif

