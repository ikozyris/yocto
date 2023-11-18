#include "headers.h"
#include <iterator>

// \param a visible position in array
// \return real position in array
// \example file[map(position)]
#define map(a)	((a))

#define gapchar 0
#define array_size 100

template<typename T>
struct gap_buf {
	unsigned capacity; // allocated size
	unsigned length;	 // length of line

	T &operator[](unsigned i) {
		return buffer[i];
	}

	// constructor
	gap_buf() {
		buffer = (T*)malloc(sizeof(T) * array_size);
		if (!buffer)
			exit(EXIT_FAILURE);
		//buffer = new T[array_size];
		length = 0;
		capacity = array_size;
		//bzero(buffer, array_size);
		memset(buffer, gapchar, array_size);
	}
	// destructor
	~gap_buf() {
		//delete buffer; 
		::free(buffer);
	}

	void resize(unsigned size);

	void insert(unsigned pos, T ch);
	//void insert(T *str, unsigned pos);

	//void push_back(T ch);
	void push_back(T *str, unsigned size);

	void erase(unsigned pos);
	//void erase(unsigned pos, unsigned size);
	T *buffer;
};

template<typename T>
void gap_buf<T>::resize(unsigned size) {
	if (size <= capacity)
		return;
	size++;
	buffer = (T*)realloc(buffer, sizeof(T) * size);
	if (!buffer)
		exit(EXIT_FAILURE);
	capacity = size;
}

template<typename T>
void gap_buf<T>::insert(unsigned pos, T ch) {
	if (buffer[map(pos)] == gapchar) {
		buffer[map(pos)] = ch;
	} else if (map(pos) < capacity) {
		for (unsigned i = length; i > map(pos); --i) {
			buffer[i] = buffer[i - 1];
		}
		buffer[map(pos)] = ch;
	} else {
		resize(capacity * 2);
	}
	++length;
}

/*template<typename T>
void gap_buf<T>::push_back(T ch) {
	if (length >= capacity)
		resize(capacity * 2);
	buffer[map(length)] = ch;
	++length;
}*/

template<typename T>
void gap_buf<T>::push_back(T *str, unsigned size) {
	if (length + size >= capacity)
		resize(capacity * 2);
	for (unsigned i = length; i < length + size; ++i)
		buffer[map(i)] = str[i - length];
	length += size;
}

template<typename T>
void gap_buf<T>::erase(unsigned pos) {
	buffer[map(pos)] = gapchar;
}
