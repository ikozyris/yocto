#include "sizes.c"

#define name "Yocto 0.8-alpha3"

void init_curses()
{
	initscr();
	cbreak();
	raw();
	noecho(); // only temporarly enabled when asking for input in header
}

void init_text()
{
	text_win = newwin(maxy - 1, maxx - 4, 1, 4);
	scrollok(text_win, TRUE);
	keypad(text_win, TRUE);
	wmove(text_win, 0, 0);
}

void print_lines()
{
	short i = maxy;
	do
		mvwprintw(ln_win, i - 1, 0, "%3d", i);
	while (--i != 0);
}

void init_lines()
{
	ln_win = newwin(maxy, 4, 1, 0);
	wmove(ln_win, 0, 0);
	wattrset(ln_win, A_DIM);
	scrollok(ln_win, TRUE);
	print_lines();
}

void clear_header()
{
	wmove(header_win, 0, 0);
	for (short i = maxx;  i != 0; --i)
		waddch(header_win, ' ');
	wrefresh(header_win);
}

inline void print_header_title()
{
	mvwprintw(header_win, 0, maxx / 2 - sizeof(name)/2, "%s", name);
	wrefresh(header_win);
}

inline void reset_header()
{
	clear_header();
	print_header_title();
}

void init_header()
{
	// newwin arguments: height, width, start y, start x
	header_win = newwin(1, maxx, 0, 0);
	wattrset(header_win, A_STANDOUT);
	reset_header();
}
