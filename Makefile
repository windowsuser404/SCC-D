CXX = g++
CXXFLAGS = -Wall -g -Wextra -Wconversion -pedantic -std=gnu11 -fopenmp

all: scc

scc:
	$(CXX) $(CXXFLAGS) -o scc scc_main.cpp

clean:
	rm scc
