/*
hough.cpp (part of Ara, https://github.com/rdujardin/Ara)

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

#include "hough.h"

using namespace std;
using namespace cv;

Hough::Hough(Timer& timer, vector<Vec3f>& circles) : Timable(timer), _circles(circles) {}

void Hough::apply(Mat& src, Mat& dst) {
	timerStart();
	HoughCircles(src, _circles, CV_HOUGH_GRADIENT, 1, src.rows/4, 100, 15); //100,22
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

