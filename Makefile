CC=g++
CFLAGS= -std=c++14 -g -Wall
SOURCE= src/*.cpp src/*.h src/searchclient/*.cpp src/searchclient/*.h src/strategies/*.h src/strategies/*.cpp src/searchengine/*.cpp
TEST= test/*.cpp
TEST_DIR= test
GTEST_DIR = googletest

.PHONY: all clean test

main: $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o environment/client/$@

gtest: $(GTEST_DIR)/src/gtest-all.cc
	g++ -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -pthread -c $^
	ar -rv libgtest.a gtest-all.o

all: main 

clean:
	rm -f main libgtest.a gtest-all.o $(TEST_DIR)/fullTest

test: gtest $(TEST) $(SOURCE) 
	g++ -isystem $(GTEST_DIR)/include -pthread $(TEST) $(SOURCE) libgtest.a -o $(TEST_DIR)/fullTest 
	test/fullTest
