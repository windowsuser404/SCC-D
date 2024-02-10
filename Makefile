CXX = g++
CXXFLAGS = -Wall -fopenmp -g -Wextra -Wconversion -pedantic -std=gnu11

all: scc

scc:
	$(CXX) $(CXXFLAGS) -o scc scc_main.cpp

clean:
	rm scc
