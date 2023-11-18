#include "gapbuffer.h"

using namespace std;

#define MAX_LINES 4

WINDOW *header_win, *ln_win, *text_win;

#include <deque>
unsigned txt_cpt = MAX_LINES; // deque does not expose this property itself
deque<gap_buf<wchar_t>> text(MAX_LINES);
deque<gap_buf<wchar_t>>::iterator it;

wchar_t s[CCHARW_MAX];
unsigned char y, x;
// offset in y axis of text and screen
long signed int ofy;
size_t ry;
unsigned char prevy;
unsigned maxy, maxx; // to store the maximum rows and columns
unsigned short i;
int ch;
char filename[FILENAME_MAX];

// file pointers input/output
FILE *fi;
FILE *fo;

// indx: tmp for lenght of line
// curnum: total lines
size_t indx, curnum;
signed int mx;
