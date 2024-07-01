#include "gapbuffer.hpp"
#include "headers.h"

#if defined(DEBUG)
#define DEFAULT_LINES 2
#else
#define DEFAULT_LINES 16
#endif
unsigned txt_cpt = DEFAULT_LINES; // capacity
std::list<gap_buf> text(DEFAULT_LINES);
std::list<gap_buf>::iterator it;

WINDOW *header_win, *ln_win, *text_win;
wchar_t s[6];
char s2[6];
unsigned char y, x, rx, len;
// offset in y axis of text and screen
long ofy;
unsigned ry;
unsigned buf_indx, printed, previndx, ppi;
unsigned maxy, maxx; // to store the maximum rows and columns
int ch;
char *filename;

// total lines
size_t curnum;
