CXX=g++
CXXFLAGS=-std=c++11

SRCDIR=src
BUILDDIR=build
EXECDIR=bin

TARGET=bin/fatman
TEST_TARGET=bin/generate-test

OBJECT=src/main.cpp
TEST_OBJECT=src/Test/test_generator.cpp

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(EXECDIR)
	
fatman: ${OBJECT}
	$(CXX) $(CXXFLAGS) ${OBJECT} -o ${TARGET}
	
test: ${TEST_OBJECT}
	$(CXX) $(CXXFLAGS) ${TEST_OBJECT} -o ${TEST_TARGET}
doc:
	$(DOX) Doxyfile