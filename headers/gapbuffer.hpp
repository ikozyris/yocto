#include "headers.h"

#if defined(UNICODE)
#define tp wchar_t
#else
#define tp char
#endif
#define gapchar 0
#if defined(DEBUG)
#define gap 2
#define array_size 8
#else
#define gap 32
#define array_size 1024
#endif
#define gaplen(a) ((a).gpe - (a).gps + 1)

struct gap_buf {
	unsigned cpt; // allocated size
	unsigned len;	 // length of line
	unsigned gps;	// gap start (first element of gap)
	unsigned gpe;	// gap end	(last element of gap)
	tp *buffer;

	tp &operator[](const unsigned pos) const {
		return buffer[pos];
	}

	gap_buf() {
		buffer = (tp*)malloc(sizeof(tp) * array_size);
		len = 0;
		gps = 0;
		gpe = array_size;
		cpt = array_size;
		memset(buffer, gapchar, array_size);
	}

};

void init(gap_buf &a)
{
	a.buffer = (tp*)malloc(sizeof(tp) * array_size);
	a.len = 0;
	a.gps = 0;
	a.gpe = array_size;
	a.cpt = array_size;
	memset(a.buffer, gapchar, array_size);
}

void resize(gap_buf &a, const unsigned size)
{
	if (a.gpe == a.cpt)
		a.gpe = size;
	a.buffer = (tp*)realloc(a.buffer, sizeof(tp) * size);
	a.cpt = size;
}

void grow_gap(gap_buf &a, const unsigned pos)
{
	if (a.len + gap >= a.cpt)
		resize(a, a.cpt * 2);
	char tmp = a[pos];
	// TODO: split into chunks and parallel
	for (unsigned i = a.len; i > pos; --i) {
		a[i + gap] = a[i];
		a[i] = gapchar;
	}
	a.gps = pos;
	a.gpe = pos + gap;
	a[pos] = gapchar;
	a[pos + gap] = tmp;
}

void mv_curs(gap_buf &a, const unsigned pos)
{
	if (a.gps == a.gpe) [[unlikely]]
		grow_gap(a, pos);
	// TODO: parallel
	else if (pos > a.gps) { // move to right
		while (pos != a.gps) {
			a.gpe++;
			a[a.gps] = a[a.gpe];
			a[a.gpe] = gapchar;
			a.gps++;
		}
	} else if (pos < a.gps) { // move to left
		while (pos != a.gps) {
			a.gps--;
			a[a.gpe] = a[a.gps];
			a[a.gps] = gapchar;
			a.gpe--;
		}
	}
}

void insert_c(gap_buf &a, const unsigned pos, const tp ch)
{
	if (a.len + gap >= a.cpt) [[unlikely]]
		resize(a, a.cpt * 2);
	if (a[pos] == gapchar) { [[likely]]
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
	if (a.len + gap + len >= a.cpt) [[unlikely]]
		resize(a, a.cpt * 2);
	if (gaplen(a) <= len)
		grow_gap(a, pos);
	//#pragma omp parallel for if(len > 10)
	for (unsigned i = pos; i < pos + len; ++i)
		a[i] = str[i - pos];

	a.len += len;
	a.gps += len;
}

void apnd_c(gap_buf &a, const tp ch)
{
	if (a.len + gap >= a.cpt) [[unlikely]]
		resize(a, a.cpt * 2);
	a[a.len] = ch;
	++a.len;
	++a.gps;
}

void apnd_s(gap_buf &a, const tp *str, const unsigned size)
{
	if (a.len + size >= a.cpt) [[unlikely]]
		resize(a, a.cpt + size * 2);
	#pragma omp parallel for
	for (unsigned i = a.len; i < a.len + size; ++i)
		a[i] = str[i - a.len];
	a.len += size;
	a.gps = a.len;
}

void apnd_s(gap_buf &a, const tp *str)
{
	unsigned i = a.len;
	while (str[i - a.len] != 0) {
		a[i] = str[i - a.len];
		if (++i >= a.cpt)
			resize(a, a.cpt * 2); 
	}
	a.len += i - a.len;
	a.gps += a.len;
}

// TODO: this does not work for multi byte chars
void eras(gap_buf &a, const unsigned pos)
{
	a[pos] = gapchar;
	a.gps--;
	a.len--;
}

// TODO: this is a mess
tp *data(const gap_buf &a, const unsigned from, const unsigned to)
{
	tp *tmp = (tp*)malloc(sizeof(tp) * (to - from + 10));
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

tp *data2(const gap_buf &a, const unsigned from, const unsigned to) {
	tp *buffer = (tp*)malloc(sizeof(tp) * a.len+4);
	for (unsigned i = 0; i < a.gps; ++i)
		buffer[i] = a[i];
	for (unsigned i = a.gpe+1; i < a.len+a.gpe; ++i)
		buffer[i-gaplen(a)] = a[i];

	tp *output = (tp*)malloc(sizeof(tp)*(to-from+1));
	for (unsigned i = from; i < to; ++i)
		output[i-from] = buffer[i];
	free(buffer);
	return output;
}

// TODO: fix this
void shrink(gap_buf &a)
{
	mv_curs(a, a.len);
	a.buffer = (tp*)realloc(a.buffer, sizeof(tp) * a.len);
}	
