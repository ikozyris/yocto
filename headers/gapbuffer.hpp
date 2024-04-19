#include "headers.h"

#if defined(DEBUG)
#define gap 2
#define array_size 8
#else
#define gap 8
#define array_size 32
#endif
#define gaplen(a) ((a).gpe - (a).gps + 1)
#define ingap(pos) ((pos >= a.gps && pos <= a.gpe) ? true : false)

struct gap_buf {
	unsigned cpt; // allocated size
	unsigned len;	 // length of line
	unsigned gps;	// gap start (first element of gap)
	unsigned gpe;	// gap end	(last element of gap)
	char *buffer;

	char &operator[](const unsigned pos) const {
		return buffer[pos];
	}

	gap_buf() {
		buffer = (char*)malloc(sizeof(char) * array_size);
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
	a.buffer = (char*)malloc(sizeof(char) * array_size);
	a.len = 0;
	a.gps = 0;
	a.gpe = array_size;
	a.cpt = array_size;
}

void resize(gap_buf &a, const unsigned size)
{
	if (a.gpe == a.cpt)
		a.gpe = size;
	a.buffer = (char*)realloc(a.buffer, sizeof(char) * size);
	a.cpt = size;
}

void grow_gap(gap_buf &a, const unsigned pos)
{
	if (a.len == a.cpt) {
		resize(a, a.cpt * 2);
		return; // let mv_curs do the work
	}
	char tmp = a[pos];
	// TODO: split into chunks and parallel
	for (unsigned i = a.len; i > pos; --i)
		a[i + gap] = a[i];
	a.gps = pos;
	a.gpe = pos + gap;
	a[pos + gap] = tmp;
}

void mv_curs(gap_buf &a, const unsigned pos)
{
	if (a.gps == a.gpe) [[unlikely]]
		grow_gap(a, pos);
	const unsigned _s = gaplen(a);
	// TODO: parallel and benchmark custom vs memmove
	if (pos > a.gps) { // move to right
		for (unsigned i = a.gps; i < pos + _s; ++i)
			a[i] = a[i + _s];
	} else if (pos < a.gps) { // move to left
		for (unsigned i = a.gpe; i >= pos + _s; --i)
			a[i] = a[i - _s];
	}
	if (pos >= a.len)
		a.gpe = a.cpt;
	else
		a.gpe = pos + _s - 1;
	a.gps = pos;
}

void insert_c(gap_buf &a, const unsigned pos, const char ch)
{
	if (a.len == a.cpt) [[unlikely]]
		resize(a, a.cpt * 2);
	if (ingap(pos)) { [[likely]]
		a[pos] = ch;
		++a.gps;
	} else {
		grow_gap(a, pos);
		a[pos] = ch;
	}
	++a.len;
}

void insert_s(gap_buf &a, const unsigned pos, const char *str, unsigned len)
{
	// is this uneeded? (later it is also checked indirectly)
	if (a.len + len >= a.cpt) [[unlikely]]
		resize(a, a.cpt * 2);
	if (gaplen(a) <= len)
		grow_gap(a, pos);
	//#pragma omp parallel for if(len > 10)
	for (unsigned i = pos; i < pos + len; ++i)
		a[i] = str[i - pos];

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
		resize(a, a.cpt + size * 2);
	#pragma omp parallel for
	for (unsigned i = a.len; i < a.len + size; ++i)
		a[i] = str[i - a.len];
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

// TODO: this does not work for multi byte chars
inline void eras(gap_buf &a, const unsigned pos)
{
	a.gps--;
	a.len--;
}

// TODO: this is a mess
const char *data(const gap_buf &a, const unsigned from, const unsigned to)
{
	char *tmp = (char*)malloc(sizeof(char) * (to - from + 10));
	bzero(tmp, to - from);
	if (a.len == 0)
		return tmp;
	if (a.gps == a.len && a.gpe == a.cpt) { // gap ends at end so don't bother
		for (unsigned i = from; i < to && i < a.gps; ++i)
			tmp[i - from] = a[i];
	} else if (a.gps == 0) {
		for (unsigned i = from + a.gpe + 1; i < to + a.gpe; ++i)
			tmp[i - a.gpe - 1] = a[i];
	} else {
		if (from < a.gps) {
			for (unsigned i = from; i <= a.gps; ++i)
				tmp[i] = a[i];
			for (unsigned i = a.gpe + 1; i <= gaplen(a) + to; ++i)
				tmp[i - gaplen(a)] = a[i];
		} else {
			for (unsigned i = gaplen(a) + a.gps; i < gaplen(a) + to; ++i)
				tmp[i - gaplen(a) - 1] = a[i];
		}
	}
	return tmp;
}

// naive implementation of above function 
const char *data2(const gap_buf &a, const unsigned from, const unsigned to) {
	char *buffer = (char*)malloc(sizeof(char) * a.len+4);
	for (unsigned i = 0; i < a.gps; ++i)
		buffer[i] = a[i];
	for (unsigned i = a.gpe+1; i < a.len+a.gpe; ++i)
		buffer[i-gaplen(a)] = a[i];

	char *output = (char*)malloc(sizeof(char)*(to-from+1));
	for (unsigned i = from; i < to; ++i)
		output[i-from] = buffer[i];
	free(buffer);
	return output;
}

// TODO: fix this
void shrink(gap_buf &a)
{
	mv_curs(a, a.len);
	a.buffer = (char*)realloc(a.buffer, sizeof(char) * a.len);
}	
