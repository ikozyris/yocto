#include "gapbuffer.hpp"
#include "headers.h"

#if defined(DEBUG)
#define DEFAULT_LINES 2
#else
#define DEFAULT_LINES 16
#endif
unsigned txt_cpt = DEFAULT_LINES; // alloc'ed nodes

list<gap_buf> text(DEFAULT_LINES);
list<gap_buf>::iterator it;

// displayed characters of previous wrap (or cut|slice of line)
#define dchar first
// bytes printed in current wrap
#define byte second
deque<pair<unsigned, unsigned>> wrap;

WINDOW *header_win, *ln_win, *text_win;
wchar_t s[4];
char s2[4];
unsigned flag; // actual x processed by offset funcs in size.c
unsigned short y, x, len;
long ofy; // offset in y axis of text and screen, x axis is in gapbuffer
unsigned ry, rx; // x, y positions in buffer/list
unsigned maxy, maxx; // to store the maximum rows and columns
int ch;
char *filename;

// total lines
size_t curnum;
