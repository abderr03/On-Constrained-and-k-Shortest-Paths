CXX = g++
CXXFLAGS = -Wall -O3

EIGEN = /usr/local/eigen-3.4.0

OBJECTS_COMMON = utils.o
SOURCES_COMMON = utils.cpp

OBJECTS_TASKS = 1-task.o 2-task.o 3-task.o 4-task.o
SOURCES_TASKS = 1-task.cpp 2-task.cpp 3-task.cpp 4-task.cpp

main: $(OBJECTS_TASKS) $(OBJECTS_COMMON)
	$(CXX) $(CXXFLAGS) -o main $(OBJECTS_TASKS) $(OBJECTS_COMMON) -I$(EIGEN) main.cpp -fopenmp

utils.o: utils.cpp
	$(CXX) -c $(CXXFLAGS) utils.cpp

1-task.o: 1-task.cpp
	$(CXX) -c $(CXXFLAGS) 1-task.cpp -fopenmp

2-task.o: 2-task.cpp
	$(CXX) -c $(CXXFLAGS) 2-task.cpp

3-task.o: 3-task.cpp
	$(CXX) -c $(CXXFLAGS) 3-task.cpp

4-task.o: 4-task.cpp
	$(CXX) -c $(CXXFLAGS) -I$(EIGEN) 4-task.cpp

clean:
	rm -f *.o
	rm -f main *~
