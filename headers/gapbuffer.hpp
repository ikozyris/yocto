#include "headers.h"

#if defined(UNICODE)
#define tp wchar_t
#else
#define tp char
#endif

#define gap 16
#define gapchar 1
#define array_size 32

typedef struct gap_buf {
	unsigned capacity; // allocated size
	unsigned length;	 // length of line
	unsigned gps;	// gap start (first element of gap)
	unsigned gpe;	// gap end	(last element of gap)
	tp *buffer;

	tp &operator[](unsigned pos) {
		return buffer[pos];
	}

	gap_buf() {
		buffer = (tp*)malloc(sizeof(tp) * array_size);
		length = 0;
		gps = 0;
		gpe = 0;
		capacity = array_size;
		//bzero(buffer, array_size);
		memset(buffer, gapchar, array_size);
	}
} gap_buf;

inline void resize(gap_buf &a, unsigned size) {
	a.buffer = (tp*)realloc(a.buffer, sizeof(tp) * size);
	a.capacity = size;
}

void grow_gap(gap_buf &a, unsigned pos) {
	char tmp = a[pos];
	// TODO: make it parallel
	for (unsigned i = a.length; i > pos; --i) {
		a[i + gap] = a[i];
		a[i] = gapchar;
	}
	a.gps = pos;
	if (a[pos + gap == gapchar])
		a.gpe = pos + gap;
	else
		a.gpe = pos + gap - 1;
	a[pos] = gapchar;
	a[pos + gap] = tmp;
}

void mv_curs(gap_buf &a, unsigned pos) {
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

inline void insert(gap_buf &a, unsigned pos, tp ch) {
	if (a.length + gap * 3 > a.capacity) [[unlikely]]
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

inline void apnd_c(gap_buf &a, tp ch) {
	if (a.length >= a.capacity) [[unlikely]]
		resize(a, a.capacity * 2);
	a[a.length] = ch;
	++a.length;
}

inline void apnd_s(gap_buf &a, tp *str, unsigned size) {
	if (a.length + size >= a.capacity) [[unlikely]]
		resize(a, a.capacity + size * 2);
	#pragma omp parallel for
	for (unsigned i = a.length; i < a.length + size; ++i)
		a[i] = str[i - a.length];
	a.length += size;
}

inline void eras(gap_buf &a, unsigned pos) {
	a[pos] = gapchar;
	a.gps--;
}

inline tp *data(gap_buf &a, unsigned width) {
	tp *tmp = (tp*)malloc(sizeof(tp) * width);
	bzero(tmp, width);
	unsigned done = 0;
	for (unsigned i = 0; done < width; ++i) {
		if (a[i] != gapchar) {
			tmp[done] = a[i];
			done++;
		}
	}
	return tmp;
}
