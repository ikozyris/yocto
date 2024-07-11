#include "headers.h"

long signed ofx;

#if defined(DEBUG)
#define gap 2
#define array_size 8
#else
#define gap 8
#define array_size 32
#endif
#define gaplen(a) ((a).gpe - (a).gps + 1)
#define ingap(a, pos) (((pos) >= (a).gps && (pos) <= (a).gpe) ? true : false)
#define mveras(a, pos) (mv_curs(a, pos), eras(a))
#define min(a, b) (a <= b ? a : b)
#define max(a, b) (a >= b ? a : b)

struct gap_buf {
	unsigned cpt; 	// allocated size
	unsigned len;	// length of line
	unsigned gps;	// gap start (first element of gap)
	unsigned gpe;	// gap end (last element of gap)
	char *buffer;

	char &operator[](const unsigned pos) const {
		return buffer[pos];
	}

	gap_buf() {
		buffer = (char*)malloc(array_size);
		len = 0;
		gps = 0;
		gpe = array_size;
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
	a.gpe = array_size;
	a.cpt = array_size;
}

void resize(gap_buf &a, const unsigned size)
{
	if (a.gpe == a.cpt)
		a.gpe = size;
	a.buffer = (char*)realloc(a.buffer, size);
	a.cpt = size;
}

void grow_gap(gap_buf &a, const unsigned pos)
{
	if (a.len >= a.cpt) {
		resize(a, a.cpt * 2);
		return; // let mv_curs do the work
	}
	char tmp = a[pos];
	memmove(a.buffer + pos + gap, a.buffer + pos, a.len - pos + 1);
	a.gps = pos;
	a.gpe = pos + gap;
	a[pos + gap] = tmp;
}

void mv_curs(gap_buf &a, const unsigned pos)
{
	if (a.gps == a.gpe) [[unlikely]]
		grow_gap(a, pos);
	const unsigned gpl = gaplen(a);
	if (pos > a.gps) // move gap to right
		memmove(a.buffer + a.gps, a.buffer + a.gps + gpl, pos - a.gps);
	else if (pos < a.gps) // move gap to left
		memmove(a.buffer + pos + gpl, a.buffer + pos, a.gps - pos);
	if (pos >= a.len)
		a.gpe = a.cpt;
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
	a.gps += a.len;
}

inline void eras(gap_buf &a)
{
	if (a[a.gps - 1] < 0) { // unicode
		--a.gps;
		--a.len;
		--ofx;
	}
	--a.gps;
	--a.len;
}

// TODO: this is a mess
char *data(const gap_buf &a, const unsigned from, const unsigned to)
{
	char *tmp = (char*)malloc(to - from + 10);
	if (a.len == 0) {
		tmp[0] = 0;
		return tmp;
	}
	bzero(tmp, to - from + 1);
	// try some special cases where 1 copy is required
	if (a.gps == a.len && a.gpe == a.cpt) // gap ends at end so don't bother
		memcpy(tmp, a.buffer + from, min(to, a.gps));
	else if (a.gps == 0) // x = 0; gap at start
		memcpy(tmp, a.buffer + from + a.gpe + 1, min(to, a.len) - from);
	else {
		if (from < a.gps) { // worse case
			for (unsigned i = from; i <= a.gps; ++i)
				tmp[i] = a[i];
			for (unsigned i = a.gpe + 1; i <= gaplen(a) + to; ++i)
				tmp[i - gaplen(a)] = a[i];
			//memcpy(tmp, a.buffer + from, min(to, a.gps));
			//memcpy(tmp + min(to, a.gps), a.buffer +, a.gps - to - 1);
		} else
			for (unsigned i = gaplen(a) + a.gps; i < gaplen(a) + to; ++i)
				tmp[i - gaplen(a) - 1] = a[i];
			//memcpy(tmp, a.buffer + a.gpe + 1, a.gps - to);
	}
	tmp[to - from + 2] = 0;
	return tmp;
}

// naive, simplier, slower, should be bug free, implementation of above function 
char *data2(const gap_buf &a, const unsigned from, const unsigned to) {
	char *buffer = (char*)malloc(a.len + 1);
	memcpy(buffer, a.buffer, a.gps);
	memcpy(buffer + a.gps, a.buffer + a.gpe + 1, a.len - a.gps);

	char *output = (char*)malloc(to - from + 2);
	memcpy(output, buffer + from, to - from + 1);
	output[to - from + 2] = 0;
	free(buffer);
	return output;
}

// TODO: fix this
void shrink(gap_buf &a)
{
	mv_curs(a, a.len);
	resize(a, a.len + 1);
}	
