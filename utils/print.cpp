#include "../headers.h"

const char *name = "Yocto 0.8-alpha3";

void print_header(WINDOW *&header_win, int &maxx)
{
	wmove(header_win, 0, 0);
        for (int i = maxx;  i != 0; --i)
                waddch(header_win, ' ');
        mvwprintw(header_win, 0, maxx / 2 - 9, "%s", name);
        wrefresh(header_win);
}

void print2header(WINDOW *&header_win, int &maxx, const char *msg, uint8_t pos)
{
        if (pos == 3) {
                mvwprintw(header_win, 0, maxx - strlen(msg), "%s", msg);
        } else if (pos == 1) {
                mvwprintw(header_win, 0, 0, "%s", msg);
        } else {
                mvwprintw(header_win, 0, maxx / 2 - 10, "                    ");
                mvwprintw(header_win, 0, maxx / 2 - strlen(msg), "%s", msg);
        }
        wrefresh(header_win);
}

void print_text(std::vector<__gnu_cxx::rope<wchar_t>> &text, 
                WINDOW *&text_win, int &maxx, int &maxy)
{
        wclear(text_win);
        wchar_t s[2] = {0, 0};
        int8_t indx = 0; int8_t y = 0; int8_t mx = -1;
        for (int8_t i = 0; i < maxy; ++i) {
        	for (const wchar_t c : text[i]) {
        		s[0] = c;
        		if (mx != 0 && indx < (uint8_t)maxx)
        			waddnwstr(text_win, s, 1);
        		else if (y == maxy && c != L'\n')
        			waddnwstr(text_win, s, 1);
        		if (c == '\n') {
        			++y;
        			if (mx != 0 && y == maxy - 1)
        				mx = 0;
        			indx = 0;
        		}
        		++indx;
        	}
        }
}