#include "io.cpp"

void init_curses()
{
	initscr();
	cbreak();
	raw();
	noecho();
}

void init_header()
{
	header_win = newwin(1, maxx, 0, 0);
	wattrset(header_win, A_STANDOUT);
	print_header();
}

void init_lines()
{
	ln_win = newwin(maxy, 4, 1, 0);
	wmove(ln_win, 0, 0);
	wattrset(ln_win, A_DIM);
	scrollok(ln_win, TRUE);
	print_lines();
}

void init_text()
{
	text_win = newwin(maxy - 1, maxx, 1, 4);
	scrollok(text_win, TRUE);
	keypad(text_win, TRUE);
	wmove(text_win, 0, 0);
}