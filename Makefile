CC=g++
CUDA_CC=nvcc
CUDA_CC_FLAGS=-O3 -arch=sm_35 -lcudadevrt -rdc=true
CC_FLAGS=-std=c++0x -Wall

all: rabinkarp_v1.out rabinkarp_v2.out rabinkarp_v3.out

debug: CC_FLAGS += -g -DDEBUG
debug: CUDA_CC_FLAGS += -g -DDEBUG
debug: rabinkarp_v1.out rabinkarp_v2.out rabinkarp_v3.out

rabinkarp_v1.out: src/rabin_karp_v1.cpp
	${CC} ${CC_FLAGS} -o bin/rabin_karp_v1.out src/rabin_karp_v1.cpp

rabinkarp_v2.out: src/rabin_karp_v2.cpp
	${CC} ${CC_FLAGS} -o bin/rabin_karp_v2.out src/rabin_karp_v2.cpp

rabinkarp_v3.out: src/rabin_karp_v3.cu
	${CUDA_CC} ${CUDA_CC_FLAGS} -o bin/rabin_karp_v3.out src/rabin_karp_v3.cu


clean:
	rm -rvf bin/*
