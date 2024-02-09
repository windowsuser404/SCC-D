CXX = g++
CXXFLAGS = -Wall -fopenmp -O3

all: scc

scc:
	$(CXX) $(CXXFLAGS) -o scc scc_main.cpp

clean:
	rm scc
