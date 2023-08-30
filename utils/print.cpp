#include "../headers.h"

const char *name = "Yocto 0.8-alpha3";

void print_header(WINDOW *&header_win, int &maxx)
{
	wmove(header_win, 0, 0);
        for (int i = maxx;  i != 0; --i)
                waddch(header_win, ' ');
        mvwprintw(header_win, 0, maxx / 2 - strlen(name) / 2, "%s", name);
        wrefresh(header_win);
}

void print2header(WINDOW *&header_win, int &maxx, const char *msg)
{
        mvwprintw(header_win, 0, maxx - strlen(msg), "%s", msg);
        wrefresh(header_win);
}
