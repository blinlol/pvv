BIN_NAME=./main
GENERATES=$(BIN_NAME) $(BIN_NAME)-debug solver

INPUT?=input

files=main.cpp utils.cpp
flags=--std=c++11 -fopenmp -O3
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

solver_dot: solver_dot.cpp utils.cpp solver.cpp
	g++ $(debugFlags) -o $@ $^

solver_spmv: solver_spmv.cpp utils.cpp solver.cpp
	g++ $(debugFlags) -o $@ $^

solver_axpy: solver_axpy.cpp utils.cpp solver.cpp
	g++ $(debugFlags) -o $@ $^

solver-test: solver_dot solver_spmv solver_axpy
	./$^ $(INPUT)

test: solver
	@echo ===== one thread
	./$^ 1 1 1 1 1
	@echo ===== four threads
	./$^ 1 1 1 1 4
	@echo ===== eight threads
	./$^ 1 1 1 1 8
	@echo =====
