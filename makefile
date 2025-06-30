CC=gcc
SRC=./src/*.c ./src/screens/*.c
INC=./include
LIB=-lraylib -lGL -lm -lpthread -ldl -lrt -lX11
NAME=test

all: clean build run

clean:
	rm -f ${NAME}

build:
	${CC} -o ${NAME} ${SRC} -I ${INC} ${LIB}

run:
	./${NAME}
