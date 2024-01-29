CXX = g++
CXXFLAGS = -Wall -fopenmp -g -Wextra -Wconversion -pedantic -std=gnu11 

all: cc scc

cc:
	$(CXX) $(CXXFLAGS) -o cc cc_main.cpp

scc:
	$(CXX) $(CXXFLAGS) -o scc scc_main.cpp

clean:
	rm cc scc
