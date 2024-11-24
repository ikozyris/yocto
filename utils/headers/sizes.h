#pragma once
#include "../../headers/main.h"
char hrsize(unsigned long bytes, char *dest, unsigned short dest_cpt);
long memusg();
long whereis(const char *str, char ch);
void get_off(unsigned &x, unsigned &i, const gap_buf &buf);
long calc_offset_dis(unsigned dx, const gap_buf &buf);
long calc_offset_act(unsigned pos, unsigned i, const gap_buf &buf);
unsigned dchar2bytes(unsigned dx, unsigned from, const gap_buf &buf);
unsigned prevdchar();
