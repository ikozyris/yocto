#include "gapbuffer.hpp"
#include "headers.h"

#if defined(DEBUG)
#define DEFAULT_LINES 2
#else
#define DEFAULT_LINES 16
#endif
unsigned txt_cpt = DEFAULT_LINES; // alloc'ed nodes
std::list<gap_buf> text(DEFAULT_LINES);
std::list<gap_buf>::iterator it;

WINDOW *header_win, *ln_win, *text_win;
wchar_t s[6];
char s2[6];
unsigned short y, x, len;
long ofy; // offset in y axis of text and screen, x axis is in gapbuffer
std::deque<unsigned> wrap; // wrapped dchars TODO: make this a stack?
unsigned ry, rx; // x, y positions in buffer/list
unsigned buf_indx, printed, previndx, ppi;
unsigned maxy, maxx; // to store the maximum rows and columns
int ch;
char *filename;

// total lines
size_t curnum;
