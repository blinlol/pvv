BIN_NAME=./a.out
GENERATES=$(BIN_NAME) $(BIN_NAME)-debug solver

INPUT?=input

files=main.cpp utils.cpp
flags=--std=c++11 -fopenmp
debugFlags=$(flags) -g -o0

all: build run

build: $(files)
	mpic++ $(flags) -o $(BIN_NAME) $^

run: build
	mpiexec $(BIN_NAME) $(INPUT)

clean:
	rm -rf $(GENERATES)

debug: $(files)
	g++ $(debugFlags) -o $(BIN_NAME)-debug $^

solver: solver_test.cpp utils.cpp solver.cpp
	g++ $(debugFlags) -o $@ $^

solver-test: solver
	./$^ $(INPUT)

test: solver
	@echo ===== one thread
	./$^ 1 1 1 1 1
	@echo ===== four threads
	./$^ 1 1 1 1 4
	@echo ===== eight threads
	./$^ 1 1 1 1 8
	@echo =====
