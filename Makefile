BIN_NAME=./a.out
GENERATES=$(BIN_NAME) $(BIN_NAME)-debug

INPUT=input

files=main.cpp utils.cpp
flags=--std=c++11 -fopenmp
debugFlags=$(flags) -g -o0

all: build run

build: $(files)
	g++ $(flags) -o $(BIN_NAME) $^

run: build
	$(BIN_NAME) $(INPUT)

clean:
	rm -rf $(GENERATES)

debug: $(files)
	g++ $(debugFlags) -o $(BIN_NAME)-debug $^
