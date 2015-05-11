CC=g++
CC_FLAGS=-std=c++0x -Wall

all: rabinkarp_v1.out rabinkarp_v2.out

debug: CC_FLAGS += -g -DDEBUG
debug: rabinkarp_v1.out rabinkarp_v2.out

rabinkarp_v1.out: src/rabin_karp_v1.cpp
	${CC} ${CC_FLAGS} -o bin/rabin_karp_v1.out src/rabin_karp_v1.cpp

rabinkarp_v2.out: src/rabin_karp_v2.cpp
	${CC} ${CC_FLAGS} -o bin/rabin_karp_v2.out src/rabin_karp_v2.cpp

clean:
	rm -rvf bin/*
