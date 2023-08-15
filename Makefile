# the compiler: gcc for C program
CC = g++

# compiler flags:
#  -Wextra    turns on extra warnings
#  -Wall      turns on most, but not all, compiler warnings
#  -O3        Optimizes the code
#  -lncurses  Links to ncurses library

CFLAGS  =  -Wall -Wextra -O3

# the build target executable:
TARGET = yocto

all:
	$(CC) main.cpp -o $(TARGET) $(CFLAGS)
	cp $(TARGET) ~/.local/bin/
	$(RM) $(TARGET)

build:
	$(CC) main.c -o $(TARGET) $(CFLAGS)

install:
	cp $(TARGET) ~/.local/bin/

clean:
	$(RM) $(TARGET)
