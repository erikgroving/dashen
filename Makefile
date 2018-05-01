CC=g++
CFLAGS= -std=c++14 -Wall -g
SRC= $(wildcard src/searchclient/*.cpp src/searchengine/*.cpp src/heuristics/*.cpp src/strategies/*.cpp src/communication/*.cpp src/agent/*.cpp)

OBJECTS= $(SRC:%.cpp=%.o)

TEST= test/*.cpp
TEST_DIR= test
GTEST_DIR = googletest

.PHONY: all clean test

environment/client/main: $(OBJECTS) src/main.cpp
	rm -f environment/client/main 
	$(CC) $(CFLAGS) $(OBJECTS) src/main.cpp -o $@

main: $(OBJECTS) src/main.cpp
	rm -f main 
	$(CC) $(CFLAGS) $(OBJECTS) src/main.cpp -o $@

gtest: $(GTEST_DIR)/src/gtest-all.cc
	g++ $(CFLAGS) -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -pthread -c $^
	ar -rv libgtest.a gtest-all.o

all: environment/client/main

clean:
	rm -f environment/client/main main libgtest.a gtest-all.o $(TEST_DIR)/fullTest $(OBJECTS)

test: gtest $(TEST) $(OBJECTS)
	g++ $(CFLAGS) -isystem $(GTEST_DIR)/include -pthread $(TEST) $(OBJECTS) libgtest.a -o $(TEST_DIR)/fullTest 
	test/fullTest

%.o: %.cpp
	$(CC) $(CFLAGS) -c $*.cpp -o $@
