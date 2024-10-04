# Yocto
[![C/C++ CI](https://github.com/ikozyris/yocto/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/ikozyris/yocto/actions/workflows/c-cpp.yml)
A simple, compact and *fast* text editor using ncurses and a gap buffer, written in C++, in ~1.2k lines of code.
More information is available on the [wiki](https://github.com/ikozyris/yocto/wiki).

![A C++ file in Yocto](https://github.com/ikozyris/yocto/assets/80053394/8fa12952-272f-41e0-9535-0a77f3652286)

## Usage
```
make build
sudo make install
```

Or use the user-friendly dialog utility `wizard.sh`
which also supports configuring yocto

### Keybindings
* Save: Ctrl-S
* Exit: Ctrl-X
* Go to start of line: Ctrl-A
* Go to end of line: Ctrl-E
* Open other file Alt-R
* Calculate x offset (for UTF-8): Alt-U
* Enter built-in terminal: Alt-C
* Show info: Alt-I (also command _stats_ in built-in terminal)

### How fast is it?
Yocto is several times faster than any other text editor at reading files and other operations.
See the [benchmarks](https://github.com/ikozyris/yocto/wiki/Performance-&-Benchmarks) for more.

### About the name
Yocto is the SI (metric) prefix for 10^-24.
Since this text editor is tiny, yocto is a suitable name.

### License

Copyright (C) 2022-2024  ikozyris<br>
Copyright (C) 2022-2024  gkozyris

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

