# Yocto
A simple and compact text editor using ncurses and a gap buffer, written in C++, in ~650 lines of code.
More information is available on the [wiki](https://github.com/ikozyris/yocto/wiki).

## How to use
```
make build
make install
```

Or use the user-friendly dialog utility (`wizard.sh`)
which also supports configuring yocto

### Keybindings
* Save: Ctrl-S
* Exit: Ctrl-C
* Go to start of line: Ctrl-A
* Go to end of line: Ctrl-E
* Enter built-in terminal: Alt-C
* Show info: Alt-I (also command _stats_ in built-in terminal)

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

