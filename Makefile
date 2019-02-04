build:
	g++-7 -pthread main.cpp Master.cpp Line.cpp Object.cpp Pixel.cpp MoveableObject.cpp ObjectFiller.cpp -o main

run:
	sudo ./main
