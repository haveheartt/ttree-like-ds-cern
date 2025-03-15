CXX = g++
CXXFLAGS = -std=c++17 `root-config --cflags --libs` -larrow

all: main

main: main.o CustomTTree.o
	$(CXX) -o main main.o CustomTTree.o $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -f *.o main
