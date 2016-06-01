all: ara

clean:
	rm -rf obj/*.o

rebuild: clean
	rm -rf ara

ara: obj/application.o obj/common.o obj/main.o obj/ballDetector.o obj/camera.o obj/timer.o obj/adjustable.o obj/hsv.o obj/filters.o obj/hough.o obj/detectors.o obj/botController.o obj/botHardware.o obj/botDraw.o obj/botTrajectories.o obj/botState.o obj/logWindow.o
	g++ -o ara obj/application.o obj/common.o obj/main.o obj/ballDetector.o obj/camera.o obj/timer.o obj/adjustable.o obj/hsv.o obj/filters.o obj/hough.o obj/detectors.o obj/botController.o obj/botHardware.o obj/botDraw.o obj/botTrajectories.o obj/botState.o obj/logWindow.o `pkg-config --cflags --libs opencv` -lwiringPi -lpthread -std=c++0x -g -O4

obj/application.o: common/application.cpp
	g++ -o obj/application.o -c common/application.cpp -std=c++0x -g -O4

obj/common.o: common/common.cpp
	g++ -o obj/common.o -c common/common.cpp -std=c++0x -g -O4

obj/main.o: common/main.cpp
	g++ -o obj/main.o -c common/main.cpp -std=c++0x -g -O4

obj/ballDetector.o: vision/ballDetector.cpp
	g++ -o obj/ballDetector.o -c vision/ballDetector.cpp -std=c++0x -g -O4 

obj/camera.o: vision/camera.cpp
	g++ -o obj/camera.o -c vision/camera.cpp -std=c++0x -g -O4 

obj/timer.o: common/timer.cpp
	g++ -o obj/timer.o -c common/timer.cpp -std=c++0x -g -O4

obj/adjustable.o: common/adjustable.cpp
	g++ -o obj/adjustable.o -c common/adjustable.cpp -std=c++0x -g -O4

obj/hsv.o: vision/hsv.cpp
	g++ -o obj/hsv.o -c vision/hsv.cpp -std=c++0x -g -O4

obj/filters.o: vision/filters.cpp
	g++ -o obj/filters.o -c vision/filters.cpp -std=c++0x -g -O4

obj/hough.o: vision/hough.cpp
	g++ -o obj/hough.o -c vision/hough.cpp -std=c++0x -g -O4

obj/detectors.o: vision/detectors.cpp
	g++ -o obj/detectors.o -c vision/detectors.cpp -std=c++0x -g -O4

obj/botController.o: bot/botController.cpp
	g++ -o obj/botController.o -c bot/botController.cpp -std=c++0x -g -O4

obj/botHardware.o: bot/botHardware.cpp
	g++ -o obj/botHardware.o -c bot/botHardware.cpp -std=c++0x -g -O4

obj/botDraw.o: bot/botDraw.cpp
	g++ -o obj/botDraw.o -c bot/botDraw.cpp -std=c++0x -g -O4

obj/botTrajectories.o: bot/botTrajectories.cpp
	g++ -o obj/botTrajectories.o -c bot/botTrajectories.cpp -std=c++0x -g -O4

obj/botState.o: bot/botState.cpp
	g++ -o obj/botState.o -c bot/botState.cpp -std=c++0x -g -O4

obj/logWindow.o: common/logWindow.cpp
	g++ -o obj/logWindow.o -c common/logWindow.cpp -std=c++0x -g -O4


