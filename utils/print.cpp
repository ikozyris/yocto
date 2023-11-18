#include "../headers/vars.h"

const char name[] = "Yocto 0.8-alpha3";

void print_lines()
{
	i = maxy;
	do {
		mvwprintw(ln_win, i - 1, 0, "%3d", i);
	} while (--i != 0);
}

void print_header()
{
	wmove(header_win, 0, 0);
	for (short i = maxx;  i != 0; --i)
		waddch(header_win, ' ');
	mvwprintw(header_win, 0, maxx / 2 - 9, "%s", name);
	wrefresh(header_win);
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
#define print_line(i) waddnwstr(text_win, text[i].buffer, min(text[i].length, maxx))
#define print_line_no_nl(i) waddnwstr(text_win, text[i].buffer, min(text[i].length - 1, maxx))

void print_text()
{
	wclear(text_win);
	for (i = 0; i < min(text.size(), (size_t)maxy - 1); ++i)
		print_line(i);
		//for (int j = 0; j < (int)text[i].length && j < maxx; ++j)
		//	waddch(text_win, text[i][j]);
	print_line_no_nl(i);
}