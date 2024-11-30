#pragma once
#include "../../headers/main.h"

char hrsize(size_t bytes, char *dest, unsigned short dest_cpt); // format bytes
unsigned memusg(); // memory usage (PSS) of yocto in kilobytes
unsigned whereis(const char *str, char ch); // position of ch in str (wrapper for strch)
long calc_offset_dis(unsigned dx, const gap_buf &buf); // offset until displayed character
long calc_offset_act(unsigned pos, unsigned i, const gap_buf &buf); // offset from i until byte pos
unsigned dchar2bytes(unsigned dx, unsigned from, const gap_buf &buf); // how many bytes are dx displayed characters  
unsigned prevdchar(); // left arrow on end of tab; update offset and move cursor
