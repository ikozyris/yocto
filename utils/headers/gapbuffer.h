#pragma once
#include "../../headers/headers.h"

extern long signed ofx;
extern char *lnbuf; // temporary buffer for printing lines
extern unsigned lnbf_cpt; // linebuff capacity

#if defined(DEBUG)
#define array_size 8
#else
#define array_size 32
#endif

#define gaplen(a) ((a).gpe - (a).gps + 1)
#define ingap(a, pos) (((pos) >= (a).gps && (pos) <= (a).gpe) ? true : false)
#define mveras(a, pos) (mv_curs(a, pos), eras(a))

struct gap_buf {
	unsigned cpt; // allocated size (1-based)
	unsigned len; // length of line (1)
	unsigned gps; // gap start (first element of gap) (0-based)
	unsigned gpe; // gap end (last element of gap)    (0)
	char *buffer;

	char &operator[](unsigned pos) const {
		return buffer[pos];
	}

	gap_buf() {
		buffer = (char*)malloc(array_size);
		len = 0;
		gps = 0;
		gpe = array_size - 1;
		cpt = array_size;
	}
	~gap_buf() {
		free(buffer);
		buffer = 0;
	}
};
unsigned min(unsigned a, unsigned b);
unsigned max(unsigned a, unsigned b);
void init(gap_buf &a);
void resize(gap_buf &a, unsigned size);
void mv_curs(gap_buf &a, unsigned pos);
void insert_c(gap_buf &a, unsigned pos, char ch);
void insert_s(gap_buf &a, unsigned pos, const char *str, unsigned len);
void apnd_c(gap_buf &a, char ch);
void apnd_s(gap_buf &a, const char *str, unsigned size);
void apnd_s(gap_buf &a, const char *str);
void eras(gap_buf &a);
unsigned data(const gap_buf &src, unsigned from, unsigned to);
char at(const gap_buf &src, unsigned pos);
unsigned data2(const gap_buf &src, unsigned from, unsigned to);
unsigned shrink(gap_buf &a);
