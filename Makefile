##
# Project Title
#
# @file
# @version 0.1

CC  = gcc
OUT = jsoner

all:
	$(CC) -o $(OUT) ./src/main.c ./src/json.c

run: all
	./$(OUT)

val: all
	valgrind ./$(OUT)
# end
