#include "../headers/vars.hpp"

// convert bytes to base-10 (SI) human-readable string e.g 1000B = 1kB
char hrsize(size_t bytes, char *dest, unsigned short dest_cpt)
{
	const char suffix[] = {0, 'k', 'M', 'G', 'T'};
	unsigned char length = sizeof(suffix) / sizeof(suffix[0]);

	double dblBytes = bytes;

	unsigned i;
	for (i = 0; (bytes / 1000) > 0 && i < (unsigned)length - 1; i++, bytes /= 1000)
		dblBytes = bytes / 1000.0;

	snprintf(dest, dest_cpt, "%.02lf %cB", dblBytes, suffix[i]);
	return suffix[i];
}

// get length of line y
unsigned sizeofline(unsigned y) {
	short i = maxx - 1;
	wmove(text_win, y, i);
	while ((winch(text_win) & A_CHARTEXT) == ' ' && i >= 0)
		wmove(text_win, y, --i);
	wmove(text_win, y, i + 1);
	return i + 2;
}

// in kB
long memusg()
{
	size_t memusg = 0, tmp;
	char buffer[1024];
	FILE *file = fopen("/proc/self/smaps", "r");
	while (fscanf(file, " %1023s", buffer) == 1)
		if (strcmp(buffer, "Pss:") == 0) {
			fscanf(file, " %lu", &tmp);
			memusg += tmp;
		}
	fclose(file);
	return memusg;
}

// return offset for current line (*it) until x
long calc_offset(unsigned short target_x)
{
	char ch;
	unsigned short x = 0, i = 0;
	while (x < target_x && i < it->len) {
		ch = at(*it, i);
		if (ch == '\t')
			x += 8 - x % 8 - 1; // -1 due to x++ at end
		// asumes this utf8 code point is 2 bytes
		else if (ch < 0)
			i++; // not a real character
		x++;
		i++;
	}
	return (signed)i - (signed)x;
}