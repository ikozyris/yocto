#pragma once
#include "io.h"

void stats(); // print statistics in header
void command(); // prompt for command
void enter(); // insert enter at *it,rx and create new node for line
void eol(); // go to end of line, if necessary cut line mod window width (maxx)
void sol(); // got to start of line, reset cut, ofx
void scrolldown(); // scroll one line down, increase y offset and iterate
void scrollup(); // scroll up
enum status {CUT, NORMAL, LN_CHANGE, SCROLL, NOTHING}; // action taken by left/right
unsigned short left(); // arrow left (returns enum status)
unsigned short right(); // arrow right (return enum status)
void prnxt_word(unsigned short func(void)); // go to next->right() previous->left() word
void reset_view(); // reprint text, go to 0,0
