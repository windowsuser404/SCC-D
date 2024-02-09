CXX = g++
CXXFLAGS = -Wall -fopenmp -g

all: scc

scc:
	$(CXX) $(CXXFLAGS) -o scc scc_main.cpp

clean:
	rm scc
