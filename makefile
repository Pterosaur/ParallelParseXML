CC = g++

FLAG = -m64 -std=c++11

LIB = -lm -lpthread

WARNING = -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast

main: main.c file.c task.c job.c xml.c parseBlock.c
	$(CC) $(FLAG) $^ $(LIB) -g -o $@