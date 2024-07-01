#include "../headers/vars.hpp"

const char *itoa(long a)
{
	static char b[12];
	sprintf(b, "%ld", a);
	return b;
}

// convert bytes to base-10 human-readable string e.g 1024 = 1.024KB = 1KiB
char *hrsize(size_t bytes)
{
	char *suffix[] = {(char*)"B", (char*)"KB", (char*)"MB",
			  (char*)"GB", (char*)"TB"};
	unsigned char length = sizeof(suffix) / sizeof(suffix[0]);

	double dblBytes = bytes;

	unsigned i;
	for (i = 0; (bytes / 1000) > 0 && i < (unsigned)length - 1; i++, bytes /= 1000)
		dblBytes = bytes / 1000.0;

	static char output[16];
	sprintf(output, "%.02lf %s", dblBytes, suffix[i]);
	return output;
}

// get length of line y
unsigned sizeofline(unsigned y) {
	unsigned i = maxx - 1;
	wmove(text_win, y, i);
	while ((winch(text_win) & A_CHARTEXT) == ' ')
		wmove(text_win, y, --i);
	return i+2;	
}

// get length of line with offset by tabs
long get_offset(gap_buf &buffer)
{
	if (buffer.len == 0)
		return 0;
	char *rbuff = data(buffer, 0, buffer.len-1);
	unsigned rlen = buffer.len;
	for (unsigned i = 0; i < buffer.len && rbuff[i] != 0; ++i)
		if (rbuff[i] == L'\t')
			rlen += 7;

	return rlen;
}
