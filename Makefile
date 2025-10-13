BIN_NAME=./a.out
GENERATES=$(BIN_NAME) $(BIN_NAME)-debug

INPUT=input

all: build run

build: main.cpp
	g++ --std=c++11 -o $(BIN_NAME) $^

run: build
	$(BIN_NAME) $(INPUT)

clean:
	rm -rf $(GENERATES)

debug: main.cpp
	g++ --std=c++11 -g -o0 -o $(BIN_NAME)-debug $^
