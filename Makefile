CC=g++
CFLAGS= -std=c++14 -Wall
HEADERS=  src/heuristics/*.h src/heuristics/greedyheuristic.o
SCOBJECTS= src/searchclient/agent.o src/searchclient/blackboard.o src/searchclient/client.o

SEOBJECTS= src/searchengine/command.o src/searchengine/distance.o src/searchengine/heuristic.o \
			src/searchengine/master.o src/searchengine/predicate.o src/searchengine/searchclient.o \
			src/searchengine/state_equal_to.o src/searchengine/state.o src/searchengine/strategy.o

STROBJECTS= src/strategies/strategybfs.o src/strategies/strategydfs.o

TEST= test/*.cpp
TEST_DIR= test
GTEST_DIR = googletest

.PHONY: all clean test

environment/client/main: $(HEADERS) $(SEOBJECTS) $(SCOBJECTS) $(STROBJECTS) src/main.cpp
	rm -f environment/client/main 
	$(CC) $(CFLAGS) $(HEADERS) $(SEOBJECTS) $(SCOBJECTS) $(STROBJECTS) src/main.cpp -o $@

main: $(HEADERS) $(SEOBJECTS) $(SCOBJECTS) $(STROBJECTS) src/main.cpp
	rm -f main 
	$(CC) $(CFLAGS) $(HEADERS) $(SEOBJECTS) $(SCOBJECTS) $(STROBJECTS) src/main.cpp -o $@

gtest: $(GTEST_DIR)/src/gtest-all.cc
	g++ $(CFLAGS) -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -pthread -c $^
	ar -rv libgtest.a gtest-all.o

all: environment/client/main

clean:
	rm -f environment/client/main main libgtest.a gtest-all.o $(TEST_DIR)/fullTest $(SCOBJECTS) $(SEOBJECTS) $(STROBJECTS)

test: gtest $(TEST) $(HEADERS) $(SEOBJECTS) $(SCOBJECTS) $(STROBJECTS) 
	g++ $(CFLAGS) -isystem $(GTEST_DIR)/include -pthread $(TEST) $(HEADERS) $(SEOBJECTS) $(SCOBJECTS) $(STROBJECTS) libgtest.a -o $(TEST_DIR)/fullTest 
	test/fullTest

src/searchclient/agent.o: src/searchclient/agent.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/searchclient/blackboard.o: src/searchclient/blackboard.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/searchclient/client.o: src/searchclient/client.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/searchengine/command.o: src/searchengine/command.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/searchengine/distance.o: src/searchengine/distance.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/searchengine/heuristic.o: src/searchengine/heuristic.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/searchengine/master.o: src/searchengine/master.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/searchengine/predicate.o: src/searchengine/predicate.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/searchengine/searchclient.o: src/searchengine/searchclient.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/searchengine/searchengine.o: src/searchengine/searchengine.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/searchengine/state_equal_to.o: src/searchengine/state_equal_to.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/searchengine/state.o: src/searchengine/state.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/searchengine/strategy.o: src/searchengine/strategy.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/strategies/strategybfs.o: src/strategies/strategybfs.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/strategies/strategydfs.o: src/strategies/strategydfs.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

src/heuristics/greedyheuristic.o: src/heuristics/greedyheuristic.cpp
	$(CC) $(CFLAGS) -c $^ -o $@