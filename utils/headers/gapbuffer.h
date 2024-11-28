#pragma once
#include "../../headers/headers.h"

extern long signed ofx; // offset in x-axis
extern char *lnbuf; // temporary buffer for output of data()
extern unsigned lnbf_cpt; // lnbuf capacity

#define array_size 8
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

inline unsigned min(unsigned a, unsigned b) {return a < b ? a : b;}
inline unsigned max(unsigned a, unsigned b) {return a > b ? a : b;}
void init(gap_buf &a); // initialize the gap buffer (should already be called by constructor)
void resize(gap_buf &a, unsigned size); // resize the buffer
void mv_curs(gap_buf &a, unsigned pos); // move the cursor to position
void insert_c(gap_buf &a, unsigned pos, char ch); // insert character at position
void insert_s(gap_buf &a, unsigned pos, const char *str, unsigned len); // insert string with given length at pos
void apnd_c(gap_buf &a, char ch); // append character
void apnd_s(gap_buf &a, const char *str, unsigned size); // append string with given size
void apnd_s(gap_buf &a, const char *str); // append null-terminated string
void eras(gap_buf &a); // erase the character at current cursor position
unsigned data(const gap_buf &src, unsigned from, unsigned to); // copy buffer with range to lnbuf
char at(const gap_buf &src, unsigned pos); // return character at position calculating the gap
unsigned data2(const gap_buf &src, unsigned from, unsigned to); // same as data(), different implementation
unsigned shrink(gap_buf &a); // shrink buffer to just fit size (gap = 2 bytes)
