# g++ (clang works too with -stdlib=libc++)
# C++20 minimum for branching prediction hints: [[likely]], otherwise C++11
CC = g++ --std=c++20

# Compiler flags:
#  -Ofast		Safe as no floating point arithmetic is done
#  -march=native	Add optimization flags specific to the host platform
#  -flto		Link Time Optimizations
#  -pendatic		Comply with the standards
#  -DHIGHLIGHT		Enable syntax highlighting
#  -lncursesw		Links to ncurses library for wide characters (unicode)

OPTIM = -Ofast -flto -s -march=native -DRELEASE
SIZE = -Os -s -flto -fdata-sections -ffunction-sections -Wl,--gc-sections
DEBUG = -g #-DDEBUG
#CXXFLAGS = -Wall -Wextra -pedantic-errors $(DEBUG) -DHIGHLIGHT -lncursesw # Debug only
CXXFLAGS = -Wall -Wextra -pedantic $(OPTIM) -DHIGHLIGHT -lncursesw

# the build target executable:
TARGET = yocto
PATHT = /usr/bin/
# root is not required to install
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
