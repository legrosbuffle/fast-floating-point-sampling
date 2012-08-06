#DEF:=-DPRINT_SAMPLING
all:
	g++ $(DEF) -O2 -std=c++0x -o sample sample.cpp
