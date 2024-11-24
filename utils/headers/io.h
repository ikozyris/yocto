#pragma once
#include "../../screen/headers/highlight.h"

void print2header(const char *msg, unsigned char pos);
char *input_header(const char *q);
#define clearline (wmove(text_win, y, 0), wclrtoeol(text_win))
#define mvprint_line(y, x, buffer, from, to) (wmove(text_win, y,x), print_line(buffer, from, to))
unsigned print_line(const gap_buf &buffer, unsigned from, unsigned to);
void print_text(unsigned line);
void save();
// For size see: https://github.com/ikozyris/yocto/wiki/Comments-on-optimizations#buffer-size-for-reading
#define SZ 524288 // 512 KiB
void read_fgets(FILE *fi);
void read_fread(FILE *fi);
