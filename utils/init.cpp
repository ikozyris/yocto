#include "print.cpp"

void init_curses()
{
	initscr();
	cbreak();
	raw();
	noecho();
}

void init_header(WINDOW *&header_win, int &maxx)
{
	header_win = newwin(1, maxx, 0, 0);
	wattrset(header_win, A_STANDOUT);
	print_header(header_win, maxx);
}

void init_lines(WINDOW *&ln_win, int &maxy)
{
	ln_win = newwin(maxy, 4, 1, 0);
	wmove(ln_win, 0, 0);
	wattrset(ln_win, A_DIM);
	scrollok(ln_win, TRUE);
	for (int i = 1;  i < maxy; ++i)
		mvwprintw(ln_win, i - 1, 0, "%3d", i);
}

void init_text(WINDOW *&text_win, int maxy, int maxx)
{
	text_win = newwin(maxy - 1, maxx, 1, 4);
	scrollok(text_win, TRUE);
	keypad(text_win, TRUE);
	wmove(text_win, 0, 0);
}