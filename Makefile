# The compiler: gcc
CC = g++ --std=c++20# -DUNICODE

# Compiler flags:
#  -Wall		Turns on most, but not all, compiler warnings
#  -Wextra		Turns on extra warnings
#  -Werror		Warnings will not be tolerated!
#  -03		Optimizes the code
#  -Wpadded		Warn anout bad struct packing
#  -pendatic	Strictier warnings
#  -lncursesw	Links to ncurses library for wide characters

#CXXFLAGS = -Wall -Wextra -O3 -Wpadded -pedantic -lncursesw # Debug only
CXXFLAGS = -O3 -lncursesw -fopenmp -march=native

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
