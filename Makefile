HEADERS = LoadBalancer.h
COMPILER = g++
FLAGS = -std=c++11

default: simulator

main.o: main.cpp $(HEADERS)
	${COMPILER} ${FLAGS} -c main.cpp  -o main.o

simulator: main.o
	${COMPILER} main.o -o  simulator

clean:
	-rm -f main.o
	-rm -f simulator
