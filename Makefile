SHELL=/bin/bash

FILES := $(wildcard src/*.c src/mod/*.c)

build:
	clang -o main $(FILES) -lncurses
run:
	./main
