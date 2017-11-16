CXX=g++
CLEAN=rm
CLEANFLAGS= -r
CXXFLAGS= -std=c++11

OBJECTS = src/main.cpp
TEST = test/test_generator.cpp

all: ${OBJECTS}
	$(CXX) $(CXXFLAGS) -o fat ${OBJECTS}
	
test: ${TEST}
	$(CXX) $(CXXFLAGS) -o generate_test ${TEST}
	
doc:
	$(DOX) Doxyfile