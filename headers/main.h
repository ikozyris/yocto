#pragma once
#include "../utils/headers/gapbuffer.h"

#if defined(DEBUG)
#define DEFAULT_LINES 2
#else
#define DEFAULT_LINES 16
#endif
extern unsigned txt_cpt; // alloc'ed nodes

extern list<gap_buf> text;
extern list<gap_buf>::iterator it;

// displayed characters of previous cut (or cut|slice of line)
#define dchar first
// bytes printed in current cut
#define byte second
extern deque<pair<unsigned, unsigned>> cut;

extern WINDOW *header_win, *ln_win, *text_win;
extern wchar_t s[4];
extern char s2[4];
extern unsigned flag; // actual x processed by offset funcs in size.cpppp
extern unsigned short y, x, len;
extern long ofy; // offset in y axis of text and screen, x axis is in gapbuffer
extern unsigned ry, rx; // x, y positions in buffer/list
extern unsigned maxy, maxx; // to store the maximum rows and columns
extern int ch;
extern char *filename;

// total lines
extern size_t curnum;