#include "headers.h"

#if defined(UNICODE)
#define tp wchar_t
#else
#define tp char
#endif
#define gapchar 1
#if defined(DEBUG)
#define gap 2
#define array_size 64
#else
#define gap 32
#define array_size 1024
#endif
#define gaplen(a) ((a).gpe - (a).gps + 1)

struct gap_buf {
	unsigned capacity; // allocated size
	unsigned length;	 // length of line
	unsigned gps;	// gap start (first element of gap)
	unsigned gpe;	// gap end	(last element of gap)
	tp *buffer;

	tp &operator[](const unsigned pos) const {
		return buffer[pos];
	}

	gap_buf() {
		buffer = (tp*)malloc(sizeof(tp) * array_size);
		length = 0;
		gps = 0;
		gpe = array_size;
		capacity = array_size;
		memset(buffer, gapchar, array_size);
	}

};

void init(gap_buf &a)
{
	a.buffer = (tp*)malloc(sizeof(tp) * array_size);
	a.length = 0;
	a.gps = 0;
	a.gpe = array_size;
	a.capacity = array_size;
	memset(a.buffer, gapchar, array_size);
}

inline void resize(gap_buf &a, const unsigned size) {
	if (a.gpe == a.capacity)
		a.gpe = size;
	a.buffer = (tp*)realloc(a.buffer, sizeof(tp) * size);
	tp *tmp = a.buffer;
	for (tmp += a.capacity; tmp < a.buffer + size; ++tmp)
		*tmp = gapchar;
	a.capacity = size;
}

void grow_gap(gap_buf &a, const unsigned pos) {
	if (a.length + gap >= a.capacity)
		resize(a, a.capacity * 2);
	char tmp = a[pos];
	// TODO: parallelize and optimize (mk unsigned, rm if)
	for (long int i = a.length; i >= pos; --i) {
		if (i == -1)
			break;
		a[i + gap] = a[i];
		a[i] = gapchar;
	}
	a.gps = pos;
	// TODO: fix inconsistencies
	if (a[pos + gap == gapchar])
		a.gpe = pos + gap;
	else
		a.gpe = pos + gap - 1;
	a[pos] = gapchar;
	a[pos + gap] = tmp;
}

void mv_curs(gap_buf &a, const unsigned pos) {
	if (a.gps == a.gpe) { [[unlikely]]
		grow_gap(a, pos);
	} else if (pos > a.gps) { // move to right
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

inline void insert_c(gap_buf &a, const unsigned pos, const tp ch)
{
	if (a.length + gap >= a.capacity) [[unlikely]]
		resize(a, a.capacity * 2);
	if (a[pos] == gapchar) { [[likely]]
		a[pos] = ch;
		++a.gps;
	} else {
		grow_gap(a, pos);
		a[pos] = ch;
	}
	++a.length;
}

inline void insert_s(gap_buf &a, const unsigned pos, const char *str, unsigned len)
{
	// is this uneeded? (later it is also checked indirectly)
	if (a.length + gap + len >= a.capacity) [[unlikely]]
		resize(a, a.capacity * 2);
	if (gaplen(a) <= len)
		grow_gap(a, pos);
	#pragma omp parallel for
	for (unsigned i = pos; i < pos + len; ++i)
		a[i] = str[i - pos];

	a.length += len;
	a.gps += len;
}

inline void apnd_c(gap_buf &a, const tp ch) {
	if (a.length + gap >= a.capacity) [[unlikely]]
		resize(a, a.capacity * 2);
	a[a.length] = ch;
	++a.length;
	++a.gps;
}

inline void apnd_s(gap_buf &a, const tp *str, const unsigned size)
{
	if (a.length + size >= a.capacity) [[unlikely]]
		resize(a, a.capacity + size * 2);
	#pragma omp parallel for
	for (unsigned i = a.length; i < a.length + size; ++i)
		a[i] = str[i - a.length];
	a.length += size;
	a.gps += size;
}

inline void apnd_s(gap_buf &a, const tp *str)
{
	unsigned i = a.length;
	while (str[i - a.length] != 0 && i < a.capacity) {
		a[i] = str[i - a.length];
		++i;
	}
	a.length += i - a.length;
	a.gps += i - a.length;
}

// TODO: this does not work for multi byte chars
void eras(gap_buf &a, const unsigned pos)
{
	a[pos] = gapchar;
	a.gps--;
	a.length--;
}

// TODO: this is a mess
inline tp *data(const gap_buf &a, const unsigned from, const unsigned to)
{
	tp *tmp = (tp*)malloc(sizeof(tp) * (to - from + 10));
	bzero(tmp, to - from);
	if (a.length == 0)
		return tmp;
	if (a.gps == a.length && a.gpe == a.capacity) { // gap ends at end so don't bother
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

// TODO: fix this
void shrink(gap_buf &a)
{
	mv_curs(a, a.length);
	a.buffer = (tp*)realloc(a.buffer, sizeof(tp) * a.length);
}	
