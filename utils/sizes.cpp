#include "../headers/headers.h"

inline char *itoa(int a)
{
	static char b[12];
	sprintf(b, "%d", a);
	return b;
}

static char *hrsize(uint64_t bytes)
{
	char *suffix[] = {(char*)"B", (char*)"KB", (char*)"MB", 
			  (char*)"GB", (char*)"TB"};
	char length = sizeof(suffix) / sizeof(suffix[0]);

	int i = 0;
	double dblBytes = bytes;

	if (bytes > 1024) {
		for (i = 0; (bytes / 1024) > 0 && i<length-1; i++, bytes /= 1024)
			dblBytes = bytes / 1024.0;
	}

	static char output[200];
	sprintf(output, "%.02lf %s", dblBytes, suffix[i]);
	return output;
}
