CXX=g++
CXXFLAGS=-Wall -g -std=c++11 -O3 -lpthread

RM=rm -rf
# C++ files to compile
SRCS= *.cc
# Name of executable
TARGET=exec_lab4


threadsafe:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	$(RM) *.out $(TARGET) *.dSYM
