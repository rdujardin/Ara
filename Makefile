all: provos

clean:
	rm -rf obj/*.o

rebuild: clean
	rm -rf provos

provos: obj/main.o obj/ballDetector.o obj/camera.o obj/timer.o obj/adjustable.o obj/hsv.o obj/filters.o obj/hough.o obj/detectors.o obj/botController.o
	g++ -o provos obj/main.o obj/ballDetector.o obj/camera.o obj/timer.o obj/adjustable.o obj/hsv.o obj/filters.o obj/hough.o obj/detectors.o obj/botController.o `pkg-config --cflags --libs opencv` -lwiringPi -lpthread -std=c++0x -g -O3

obj/main.o: main.cpp
	g++ -o obj/main.o -c main.cpp -std=c++0x -g -O3

obj/ballDetector.o: ballDetector.cpp
	g++ -o obj/ballDetector.o -c ballDetector.cpp -std=c++0x -g -O3 

obj/camera.o: camera.cpp
	g++ -o obj/camera.o -c camera.cpp -std=c++0x -g -O3 

obj/timer.o: timer.cpp
	g++ -o obj/timer.o -c timer.cpp -std=c++0x -g -O3

obj/adjustable.o: adjustable.cpp
	g++ -o obj/adjustable.o -c adjustable.cpp -std=c++0x -g -O3

obj/hsv.o: hsv.cpp
	g++ -o obj/hsv.o -c hsv.cpp -std=c++0x -g -O3

obj/filters.o: filters.cpp
	g++ -o obj/filters.o -c filters.cpp -std=c++0x -g -O3

obj/hough.o: hough.cpp
	g++ -o obj/hough.o -c hough.cpp -std=c++0x -g -O3

obj/detectors.o: detectors.cpp
	g++ -o obj/detectors.o -c detectors.cpp -std=c++0x -g -O3

obj/botController.o: botController.cpp
	g++ -o obj/botController.o -c botController.cpp -std=c++0x -g -O3


	
