# The compiler: g++
CC = g++ --std=c++20

# Compiler flags:
#  -Wall		Turns on most, but not all, compiler warnings
#  -Wextra		Turns on extra warnings
#  -Werror		Warnings will not be tolerated!
#  -Ofast		Optimizes the code
#  -fopenmp		Multi-threading
#  -march=native	Add optimization flags specific to the host
#  -flto		Link Time Optimization
#  -pendatic		Strictier warnings
#  -lncursesw		Links to ncurses library for wide characters

#CXXFLAGS = -Og -g -Wall -Wextra -pedantic -fopenmp -DDEBUG -lncursesw # Debug only
CXXFLAGS = -Ofast -fopenmp -march=native -flto -lncursesw
#CXXFLAGS = -Og -g -fopenmp -march=native -lncursesw

# the build target executable:
TARGET = yocto
PATHT = /usr/bin/
# if available root is not required to install
#PATHT = ~/.local/bin/

all:
	$(CC) main.cpp -o $(TARGET) $(CXXFLAGS)
	cp $(TARGET) $(PATHT)$(TARGET)

build:
	$(CC) main.cpp -o $(TARGET) $(CXXFLAGS)

install:
	cp $(TARGET) $(PATHT)$(TARGET)

uninstall:
	rm $(PATHT)$(TARGET)

clean:
	$(RM) $(TARGET)
