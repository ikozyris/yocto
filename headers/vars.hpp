#include "gapbuffer.hpp"
#include "headers.h"

#if defined(DEBUG)
#define DEFAULT_LINES 16
#else
#define DEFAULT_LINES 128
#endif
unsigned txt_cpt = DEFAULT_LINES; // capacity
std::list<gap_buf> text(DEFAULT_LINES);
std::list<gap_buf>::iterator it;

WINDOW *header_win, *ln_win, *text_win;
wchar_t s[6];
char s2[6];
unsigned char y, x, ofx = 0, len;
// offset in y axis of text and screen
long int ofy;
unsigned int ry;
unsigned int maxy, maxx; // to store the maximum rows and columns
int ch;
char *filename;

// indx: tmp for lenght of line
// curnum: total lines
size_t indx, curnum;
