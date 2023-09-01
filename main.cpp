#include "headers.h"
#include "utils/init.cpp"
#include "keybindings.h"
#include "utils/sizes.cpp"

#define MAX_LINES       120'000

using namespace std;
using namespace __gnu_cxx;

//vector<crope> text(MAX_LINES);
vector<rope<wchar_t>> text(MAX_LINES);
rope<size_t> len(MAX_LINES);
rope<size_t>::iterator it;

int y = 0, x = 0, prevy;
// offset in y axis of text and screen
int ofy = 0, ry;

int maxy = 0, maxx = 0; // to store the maximum rows and columns
uint8_t i;
wchar_t ch;
char filename[FILENAME_MAX];
wchar_t s[2]; // tmp array since there are only functions for wchar_t*

// file pointers input/output
FILE *fi;
FILE *fo;

// indx: tmp for lenght of line
// curnum: total lines
int indx = 0, curnum = 0, mx = -1;

int main(int argc, char *argv[])
{
	// UTF-8
	setlocale(LC_ALL, "");
	setlocale(LC_NUMERIC,"C");

	bzero(s, 2);
	bzero(filename, FILENAME_MAX);

	init_curses();

	getmaxyx(stdscr, maxy, maxx);

	// initialize windows
	WINDOW *header_win;
	init_header(header_win, maxx);

	WINDOW *ln_win;
	init_lines(ln_win, maxy);

	WINDOW *text_win;
	init_text(text_win, maxy, maxx);

	getmaxyx(text_win, maxy, maxx);
	wrefresh(ln_win);

	if (argc > 1) {
		strcpy(filename, argv[1]);
		fi = fopen(argv[1], "r");
		if (fi == NULL) {
			fo = fopen(filename, "w");
			goto read;
		}
		it = len.mutable_begin();
		while ((ch = fgetwc(fi)) != EOF) {
			text[curnum].push_back(ch);		
			if (ch == '\n') {
				//it[curnum] = indx - 1; // seg faults
				*(it + curnum) = indx - 1;
				indx = 0;
				++curnum;
			}
			++indx;
		}
		fclose(fi);
		for (i = 0; i <= maxy; i++) {
			if (i < maxy - 1)
				waddnwstr(text_win, text[i].c_str(), maxx);
			else if (i == maxy - 1)
				waddnwstr(text_win, text[i].c_str(), len[i]);
		}
	}

	wmove(text_win, 0, 0);
//	goto stop; /*
read:
	while (1) {
		getyx(text_win, y, x);
		ry = y + ofy; // calculate once
		if (x > (int)len[ry])
			wmove(text_win, y, len[ry]);
		wget_wch(text_win, (wint_t*)s);
		switch (s[0]) {
		case DOWN:
			if (ry > curnum) // do not scroll indefinetly
				break;
			if (y == (maxy - 1) && ry < curnum) {
				ofy++;
				wscrl(text_win, 1);
				wscrl(ln_win, 1);
				mvwprintw(ln_win, maxy - 1, 0, "%3d", ry + 2);
				wrefresh(ln_win);
				wmove(text_win, y, 0);
				for (const auto &c : text[ry + 1]) {
					if (getcurx(text_win) < maxx - 1 && c != '\n') {
						s[0] = c;
						waddnwstr(text_win, s, 1);
					}
				}
				wmove(text_win, y, x);
			} else {
				if (len[ry] > len[ry + 1])
					wmove(text_win, y + 1, x);
				else
					wmove(text_win, y + 2, len[ry + 1]);
			}
			wmove(text_win, y + 1, x);
			break;

		case UP:
			if (y == 0 && ofy != 0) {
				wscrl(text_win, -1); // scroll up
				wscrl(ln_win, -1);
				mvwprintw(ln_win, 0, 0, "%3d", ry);
				--ofy;
				wmove(text_win, 0, 0);
				for (const auto &c : text[ry - 1]) {
					if (getcurx(text_win) < maxx - 1) {
						s[0] = c;
						waddnwstr(text_win, s, 1);
					}
				}
				wmove(text_win, 0, x);
				wrefresh(ln_win);
			} if (len[ry] > len[ry - 1]) {
				wmove(text_win, y - 1, len[ry + 1]);
				mx = getcurx(text_win);
			} else {
				wmove(text_win, y - 1, x);
			}
			break;

		case LEFT:
			if (x > 0)
				wmove(text_win, y, x - 1);
			else
				wmove(text_win, y - 1, len[ry - 1]);
			break;

		case RIGHT:
			if ((size_t)x < len[ry])
				wmove(text_win, y, x + 1);
			else if (ry < curnum)
				wmove(text_win, y + 1, 0);
			break;

		case BACKSPACE:
			mvwdelch(text_win, y, x - 1);
			text[ry].erase(x - 1, 1);
			it = len.mutable_begin();
			*(it + ry)--;
			break;

		case DELETE:
			wdelch(text_win);
			text[ry].erase(x, 1);
			it = len.mutable_begin();
			*(it + ry)--;
			break;

		case ENTER:
			text[ry].insert(x, '\n');
			len.insert(x, (size_t)0);

			prevy = y;
			wclear(text_win);

			// HACK: print text again
			indx = 0; y = 0; curnum = 0; mx = -1;
			for (i = 0; i < maxy; ++i) {
				for (const wchar_t c : text[i]) {
					s[0] = c;
					if (mx != 0 && indx < maxx)
						waddnwstr(text_win, s, 1);
					if (y == maxy && c != L'\n')
						waddnwstr(text_win, s, 1);
					if (c == '\n') {
						it = len.mutable_begin();
						it += y;
						*it = indx - 1;
						++y;
						if (mx != 0 && y == maxy -1)
							mx = 0;
						indx = 0;
						++curnum;
					}
					++indx;
				}
			}
			wrefresh(text_win);
			wmove(text_win, prevy+1, x);
			break;

		case HOME:
			wmove(text_win, y, 0);
			break;

		case END:
			getmaxyx(text_win, maxy, maxx);
			--maxx;
			wmove(text_win, y, maxx);
			while ((winch(text_win) & A_CHARTEXT) == ' ')
				wmove(text_win, y, --maxx);
			wmove(text_win, y, ++maxx);
			break;

		case SAVE:
			i = 0;
			if (strlen(filename) == 0) {
				wclear(header_win);
				wmove(header_win, 0, 0);
				waddnstr(header_win, "Enter filename: ", 17);
				wrefresh(header_win);
				wmove(header_win, 0, 15);
				//wscanw(header_win, "%s", filename);
				if (wgetnstr(header_win, filename,
					     FILENAME_MAX) == ERR ||
				    strlen(filename) == 0) {
					print_header(header_win, maxx);
					print2header(header_win, maxx, "ERROR", 1);
					wmove(text_win, y, x);
					break;
				}
			}
			fo = fopen(filename, "w");
			for (i = 0; i < curnum; ++i)
				for (const auto &c : text[i])
					fputwc(c, fo);

			fclose(fo);

			print_header(header_win, maxx);
			print2header(header_win, maxx, "Saved", 1);
			wmove(text_win, y, x);
			break;

		case INFO:
			if (strlen(filename) != 0) {
				struct stat stat_buf;
				if (stat(filename, &stat_buf) == 0) {
					print2header(header_win, maxx, hrsize(stat_buf.st_size), 3);
					print2header(header_win, maxx, itoa(curnum), 1);
				}
			}
			char tmp[42];
			bzero(tmp, 42);
			strcat(tmp, "y: ");
			strcat(tmp, itoa(ry + 1));
			strcat(tmp, " x: ");
			strcat(tmp, itoa(x + 1));
			print2header(header_win, maxx, tmp, 2);
			wmove(text_win, y, x);
			break;

		case RESET:
			wmove(text_win, 0, 0);
			print_header(header_win, maxx);
			break;

		case EXIT:
			goto stop;

		default:
			wins_nwstr(text_win, s, 1);
			wmove(text_win, y, x + 1);
			text[ry].insert(x, s[0]);
			it = len.mutable_begin();
			*it++;
			break;
		}
	}
	//*/
stop:
	delwin(text_win);
	delwin(ln_win);
	delwin(header_win);
	endwin();
	return 0;
}
