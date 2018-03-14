CC=g++
CFLAGS= -std=c++14
SOURCE= src/*.cpp
TEST= test/*.cpp
TEST_DIR= test
GTEST_DIR = googletest

.PHONY: all clean test

main: $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $@ 

gtest: $(GTEST_DIR)/src/gtest-all.cc
	g++ -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -pthread -c $^
	ar -rv libgtest.a gtest-all.o

all: main 

clean:
	rm -f main libgtest.a gtest-all.o $(TEST_DIR)/fullTest

test: gtest $(TEST)
	g++ -isystem $(GTEST_DIR)/include -pthread $(TEST) libgtest.a -o $(TEST_DIR)/fullTest 
	test/fullTest