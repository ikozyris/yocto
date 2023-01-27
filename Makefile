# the compiler: gcc for C program
CC = gcc

# compiler flags:
#  -Wextra    turns on extra warnings
#  -Wall      turns on most, but not all, compiler warnings
CFLAGS  =  -Wall -Wextra

# the build target executable:
TARGET = yocto

all:
	$(CC) $(CFLAGS) main.c -o $(TARGET)
	cp $(TARGET) /usr/local/bin/
	$(RM) $(TARGET)

build:
	$(CC) $(CFLAGS) main.c -o $(TARGET)

install:
	cp $(TARGET) /usr/local/bin/

clean:
	$(RM) $(TARGET)
