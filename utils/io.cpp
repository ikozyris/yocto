#include "../headers/vars.hpp"

const char name[] = "Yocto 0.8-alpha3";

void print_lines()
{
	short i = maxy;
	do {
		mvwprintw(ln_win, i - 1, 0, "%3d", i);
	} while (--i != 0);
}

void clear_header()
{
	wmove(header_win, 0, 0);
	for (short i = maxx;  i != 0; --i)
		waddch(header_win, ' ');
	wrefresh(header_win);
}

inline void print_header_title() {
	mvwprintw(header_win, 0, maxx / 2 - 9, "%s", name);
	wrefresh(header_win);
}

inline void reset_header() {
	clear_header();
	print_header_title();
}

// pos: (1 left) (3 right) (else center)
void print2header(const char *msg, unsigned char pos)
{
	if (pos == 3) {
		mvwprintw(header_win, 0, maxx - strlen(msg), "%s", msg);
	} else if (pos == 1) {
		mvwprintw(header_win, 0, 0, "%s", msg);
	} else {
		unsigned char hmx = maxx / 2;
		mvwprintw(header_win, 0, hmx - 10, "                    ");
		mvwprintw(header_win, 0, hmx - strlen(msg), "%s", msg);
	}
	wrefresh(header_win);
}

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

#if defined(UNICODE)
#define print_line(i) waddnwstr(text_win, data(text[i], maxx), min(text[i].length, maxx))
#define print_line_no_nl(i) waddnwstr(text_win, data(text[i], maxx), min(text[i].length - 1, maxx))
#else
#define print_line(i) waddnstr(text_win, data(text[i], maxx), min(text[i].length, maxx))
#define print_line_no_nl(i) waddnstr(text_win, data(text[i], maxx), min(text[i].length - 1, maxx))
#endif

void print_text()
{
	unsigned short i;
	wclear(text_win);
	for (i = 0; i < min(txt_cpt, maxy - 1); ++i)
		print_line(i);
	print_line_no_nl(i);
}

inline void read_getc(FILE *fi)
{
#if defined(UNICODE)
	while ((ch = getwc_unlocked(fi)) != EOF) {
#else
	while ((ch = getc_unlocked(fi)) != EOF) {
#endif
		apnd_c(text[curnum], ch);
		if (ch == '\n')  { [[unlikely]]
			++curnum;
			if (curnum >= txt_cpt) { [[unlikely]]
				txt_cpt = text.size() * 2;
				text.resize(txt_cpt);
			}
		}
	}
}
#define sz 1'000'000 // 1MB temp array

inline void read_fgets(FILE *fi)
{
	tp tmp[sz];
#if defined(UNICODE)
	while ((fgetws_unlocked(tmp, sz, fi))) {
#else
	while ((fgets_unlocked(tmp, sz, fi))) {
#endif
		apnd_s(text[curnum], tmp, sz);
		if (text[curnum].capacity > 10e5) [[unlikely]]
			++curnum;
	}
}

inline void read_fread(FILE *fi)
{
	tp tmp[sz];
	size_t a = 0;
	while ((a = fread(tmp, sizeof(tmp[0]), sz, fi)))
		apnd_s(text[curnum], tmp, a);
}
