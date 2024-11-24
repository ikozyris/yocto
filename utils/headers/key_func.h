#pragma once
#include "io.h"

void stats();
void command();
void enter();
void eol();
void sol();
void scrolldown();
void scrollup();
enum status {CUT, NORMAL, LN_CHANGE, SCROLL, NOTHING};
unsigned short left();
unsigned short right();
void prnxt_word(unsigned short func(void));
void reset_view();
