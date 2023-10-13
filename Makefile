# The compiler: g++
CC = g++

# Compiler flags:
#  -Wall		Turns on most, but not all, compiler warnings
#  -Wextra		Turns on extra warnings
#  -Werror		Warnings will not be tolerated!
#  -Ofast		Optimizes the code
#  -lncursesw	Links to ncurses library for wide characters

#CXXFLAGS = -Wall -Wextra -Werror -pedantic -Ofast -lncursesw # Debug only
CXXFLAGS = -Ofast -lncursesw

# the build target executable:
TARGET = yocto

all:
	$(CC) -g main.cpp -o $(TARGET) $(CXXFLAGS)
	mv $(TARGET) ~/.local/bin/

build:
	$(CC) -g main.cpp -o $(TARGET) $(CXXFLAGS)

install:
	cp $(TARGET) ~/.local/bin/

uninstall:
	rm ~/.local/bin/$(TARGET)

clean:
	$(RM) $(TARGET)
