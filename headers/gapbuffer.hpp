#include "headers.h"

// \param a visible position in array
// \return real position in array
// \example file[map(position)]
#define map(a)	a // for future use (currently useless)

#if defined(UNICODE)
#define tp wchar_t
#else
#define tp char
#endif

#define gap 10
#define gapchar 0
#define array_size 50

typedef struct gap_buf {
	unsigned capacity; // allocated size
	unsigned length;	 // length of line
	tp *buffer;

	tp &operator[](unsigned pos) {
		return buffer[pos];
	}

	gap_buf() {
		buffer = (tp*)malloc(sizeof(tp) * array_size);
		//buffer = new tp[array_size];
		length = 0;
		capacity = array_size;
		bzero(buffer, array_size);
		//memset(buffer, gapchar, array_size);
	}
} gap_buf;

inline void init(gap_buf &a) {
	a.buffer = (tp*)malloc(sizeof(tp) * array_size);
	//buffer = new tp[array_size];
	a.length = 0;
	a.capacity = array_size;
	bzero(a.buffer, array_size);
	//memset(a.buffer, gapchar, array_size);
}

inline void resize(gap_buf &a, unsigned size) {
	//size++;
	a.buffer = (tp*)realloc(a.buffer, sizeof(tp) * size);
	a.capacity = size;

	return;
}

inline void insert(gap_buf &a, unsigned pos, tp ch) {
	if (a.buffer[map(pos)] == gapchar) {
		a.buffer[map(pos)] = ch;
	} else if (map(pos) < a.capacity) {
		/*char tmp = a.buffer[map(pos)];
		for (unsigned i = a.length; i > map(pos); --i) {
			a.buffer[i + gap] = a.buffer[i];
			a.buffer[i] = gapchar;
		}
		a.buffer[map(pos)] = ch;
		a.buffer[map(pos + gap)] = tmp;*/
		for (unsigned i = a.length; i > map(pos); --i) {
			a.buffer[i] = a.buffer[i - 1];
		}
		a.buffer[map(pos)] = ch;
	} else {
		resize(a, a.capacity * 2);
	}
	++a.length;
}

inline void apnd_c(gap_buf &a, tp ch) {
	if (a.length >= a.capacity) [[unlikely]]
		resize(a, a.capacity * 2);
	a.buffer[map(a.length)] = ch;
	++a.length;
}

inline void apnd_s(gap_buf &a, tp *str, unsigned size) {
	if (a.length + size >= a.capacity) [[unlikely]]
		resize(a, a.capacity + size * 2);
	#pragma omp parallel for
	for (unsigned i = a.length; i < a.length + size; ++i)
		a.buffer[map(i)] = str[i - a.length];
	a.length += size;
}

inline void eras(gap_buf &a, unsigned pos) {
	a.buffer[map(pos)] = gapchar;
}

inline tp *data(gap_buf &a) {
	tp *tmp = (tp*)malloc(sizeof(tp) * a.capacity);
	bzero(tmp, a.capacity);
	unsigned done = 0;
	for (unsigned i = 0; i < a.capacity; ++i) 
		if (a.buffer[i] != gapchar) {
			tmp[done] = a.buffer[i];
			done++;
		}
	return tmp;
}
