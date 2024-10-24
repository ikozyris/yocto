#include "../headers/vars.hpp"

// convert bytes to base-10 (SI) human-readable string e.g 1000B = 1kB
char hrsize(size_t bytes, char *dest, unsigned short dest_cpt)
{
	const char suffix[] = {0, 'k', 'M', 'G', 'T'};
	unsigned char length = sizeof(suffix) / sizeof(suffix[0]);

	double dblBytes = bytes;

	unsigned char i;
	for (i = 0; (bytes / 1000) > 0 && i < length - 1; ++i, bytes /= 1000)
		dblBytes = bytes / 1000.0;

	snprintf(dest, dest_cpt, "%.02lf %cB", dblBytes, suffix[i]);
	return suffix[i];
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

inline void get_off(unsigned &x, unsigned &i, const gap_buf &buf)
{
	char ch = at(buf, i);
	if (ch == '\t')
		x += 8 - x % 8 - 1; // -1 due to x++ at end
	else if (ch < 0)
		i++; // assumes this utf8 code point is 2 bytes
	x++;
	i++;
}

// offset until displayed x from from bytes in buf
long calc_offset_dis(unsigned dx, unsigned from, const gap_buf &buf)
{
	unsigned x = 0, i = from;
	while (x < dx && i < buf.len)
		get_off(x, i, buf);
	return (long)i - (long)from - (long)x;
}

// displayed characters dx to bytes
unsigned dchar2bytes(unsigned dx, unsigned from, const gap_buf &buf)
{
	unsigned x = 0, i = from;
	while (x < dx && i < buf.len)
		get_off(x, i, buf);
	return i;
}

// offset until pos bytes from i bytes in buf
long calc_offset_act(unsigned pos, unsigned i, const gap_buf &buf)
{
	unsigned x = 0;
	while (i < pos)
		get_off(x, i, buf);
	return (long)i - (long)x;
}

// Go to end of previous word and return
unsigned prevword(unsigned x, unsigned y)
{
	unsigned i = x;
	// skip current word
	while ((winch(text_win) & A_CHARTEXT) != ' ' && i > 0)
		wmove(text_win, y, --i);
	// go past any spaces
	while ((winch(text_win) & A_CHARTEXT) == ' ' && i > 0)
		wmove(text_win, y, --i);
	if ((winch(text_win) & A_CHARTEXT) == ' ') {
		return x - 1;
	}
	wmove(text_win, y, ++i);
	return x - i - 1;
}
