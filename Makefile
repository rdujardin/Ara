provos: main.cpp ballDetector.cpp camera.cpp timer.cpp adjustable.cpp hsv.cpp filters.cpp hough.cpp detectors.cpp botController.cpp
	g++ -std=c++11 -g -O3 -o provos main.cpp ballDetector.cpp camera.cpp timer.cpp adjustable.cpp hsv.cpp filters.cpp hough.cpp detectors.cpp botController.cpp `pkg-config --cflags --libs opencv` -lwiringPi -lpthread
	
