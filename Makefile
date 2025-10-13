BIN_NAME=./a.out
GENERATES=$(BIN_NAME)

INPUT=input

all: build run

build: main.cpp
	g++ --std=c++11 -o $(BIN_NAME) $^

run: build
	$(BIN_NAME) $(INPUT)

clean:
	rm -rf $(GENERATES)
