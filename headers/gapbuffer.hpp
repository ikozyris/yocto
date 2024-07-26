#include "headers.h"

long signed ofx;
char *lnbuf; // temporary buffer for printing lines
unsigned lnbf_cpt; // linebuff capacity

#if defined(DEBUG)
#define array_size 8
#else
#define array_size 32
#endif

#define gaplen(a) ((a).gpe - (a).gps + 1)
#define ingap(a, pos) (((pos) >= (a).gps && (pos) <= (a).gpe) ? true : false)
#define mveras(a, pos) (mv_curs(a, pos), eras(a))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

struct gap_buf {
	unsigned cpt; 	// allocated size (1)
	unsigned len;	// length of line (1)
	unsigned gps;	// gap start (first element of gap) (0)
	unsigned gpe;	// gap end (last element of gap)    (0)
	char *buffer;

	char &operator[](const unsigned pos) const {
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

void init(gap_buf &a)
{
	a.buffer = (char*)malloc(array_size);
	a.len = a.gps = 0;
	a.gpe = array_size - 1;
	a.cpt = array_size;
}

void resize(gap_buf &a, const unsigned size)
{
	a.buffer = (char*)realloc(a.buffer, size);
	if (a.gpe < a.cpt - 1) {
		memmove(a.buffer + size - a.gpe - 1, a.buffer + a.gpe, a.cpt - a.gpe);
		a.gpe = size - a.gpe - 1; 
	} else
		a.gpe = size - 1;
	a.cpt = size;
}

void mv_curs(gap_buf &a, const unsigned pos)
{
	if (a.gps == a.gpe) [[unlikely]]
		resize(a, a.cpt * 2);
	const unsigned gpl = gaplen(a);
	if (pos > a.gps) // move gap to right
		memmove(a.buffer + a.gps, a.buffer + a.gps + gpl, pos - a.gps);
	else if (pos < a.gps) // move gap to left
		memmove(a.buffer + pos + gpl, a.buffer + pos, a.gps - pos);
	if (pos >= a.len)
		a.gpe = a.cpt - 1;
	else
		a.gpe = pos + gpl - 1;
	a.gps = pos;
}

void insert_c(gap_buf &a, const unsigned pos, const char ch)
{
	if (a.len == a.cpt) [[unlikely]]
		resize(a, a.cpt * 2);
	if (ingap(a, pos)) { [[likely]]
		a[pos] = ch;
		++a.gps;
	} else {
		mv_curs(a, pos);
		a[pos] = ch;
	}
	++a.len;
}

void insert_s(gap_buf &a, const unsigned pos, const char *str, unsigned len)
{
	// is this uneeded? (later it is also checked indirectly)
	if (a.len + len >= a.cpt) [[unlikely]]
		resize(a, (a.cpt + len) * 2);
	if (gaplen(a) <= len)
		mv_curs(a, pos);
	memcpy(a.buffer + pos, str, len);
	a.len += len;
	a.gps += len;
}

void apnd_c(gap_buf &a, const char ch)
{
	if (a.len >= a.cpt) [[unlikely]]
		resize(a, a.cpt * 2);
	a[a.len] = ch;
	++a.len;
	++a.gps;
}

void apnd_s(gap_buf &a, const char *str, const unsigned size)
{
	if (a.len + size >= a.cpt) [[unlikely]]
		resize(a, (a.cpt + size) * 2);
	memcpy(a.buffer + a.len, str, size);
	a.len += size;
	a.gps = a.len;
}

void apnd_s(gap_buf &a, const char *str)
{
	unsigned i = a.len;
	while (str[i - a.len] != 0) {
		a[i] = str[i - a.len];
		if (++i == a.cpt)
			resize(a, a.cpt * 2); 
	}
	a.len += i - a.len;
	a.gps = a.len;
}

inline void eras(gap_buf &a)
{
	if (a[a.gps - 1] < 0) { // unicode
		--a.gps;
		--a.len;
		--ofx; // assumes this UTF-8 point is 2 bytes
	}
	--a.gps;
	--a.len;
}

// TODO: this is a mess
// NOTE: destination buffer is lnbuf
unsigned data(const gap_buf &src, unsigned from, unsigned to)
{
	if (src.len == 0)
		return 0;
	if (lnbf_cpt < to - from + 3) {
		free(lnbuf);
		lnbf_cpt = to - from + 10;
		lnbuf = (char*)malloc(lnbf_cpt);
	}
	lnbuf[to - from] = 0;
	lnbuf[to - from + 1] = 0;
	// try some special cases where 1 copy is required
	if (src.gps == src.len && src.gpe == src.cpt - 1) // gap ends at end
		memcpy(lnbuf, src.buffer + from, min(to, src.gps));
	else if (src.gps == 0) // x = 0; gap at start
		memcpy(lnbuf, src.buffer + from + src.gpe + 1, min(to, src.len) - from);
	else {
		if (from < src.gps) { // worst case
			for (unsigned i = from; i < src.gps; ++i)
				lnbuf[i] = src[i];
			for (unsigned i = src.gpe + 1; i < gaplen(src) + to; ++i)
				lnbuf[i - gaplen(src)] = src[i];
			//memcpy(tmp, src.buffer + from, min(to, src.gps));
			//memcpy(tmp + min(to, src.gps), src.buffer +, src.gps - to - 1);
		} else
			for (unsigned i = gaplen(src) + src.gps; i < gaplen(src) + to; ++i)
				lnbuf[i - gaplen(src) - 1] = src[i];
			//memcpy(tmp, src.buffer + src.gpe + 1, src.gps - to);
	}
	lnbuf[to - from + 2] = 0;
	return to - from;
}

// naive, simplier, slower, should be bug free, implementation of above function 
unsigned data2(const gap_buf &src, const unsigned from, const unsigned to) {
	char *buffer = (char*)malloc(src.len + 3);
	memcpy(buffer, src.buffer, src.gps);
	memcpy(buffer + src.gps, src.buffer + src.gpe + 1, src.len - src.gps);

	if (src.len == 0)
		return 0;
	if (lnbf_cpt < to - from + 3) {
		free(lnbuf);
		lnbf_cpt = to - from + 10;
		lnbuf = (char*)malloc(lnbf_cpt);
	}
	memcpy(lnbuf, buffer + from, to - from);
	free(buffer);
	lnbuf[to - from + 1] = 0;
	lnbuf[to - from + 2] = 0;
	return to - from;
}

// TODO: fix this
unsigned shrink(gap_buf &a)
{
	unsigned bytes = a.cpt;
	mv_curs(a, a.len);
	resize(a, a.len + 1);
	return bytes - a.cpt;
}	
