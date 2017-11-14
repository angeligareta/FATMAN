CXX=g++
CLEAN=rm
CLEANFLAGS= -r
CXXFLAGS= -std=c++11

OBJECTS = src/main.o

compareFA: ${OBJECTS}
	$(CXX) $(CXXFLAGS) -o fat ${OBJECTS}
	
doc:
	$(DOX) Doxyfile
	
clean: ${OBJECTS}
	$(CLEAN) $(CLEANFLAGS) ${OBJECTS}