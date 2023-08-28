#include "headers.h"
#include "utils/init.cpp"

#define ctrl(x)         ((x) & 0x1f)
#define MAX_LINES       120'000

#undef 	KEY_ENTER
#define KEY_ENTER       10 // overwrite default

using namespace std;
using namespace __gnu_cxx;

//vector<crope> text(MAX_LINES);
//vector<string> text(MAX_LINES);
//vector<vector<wchar_t>> text (MAX_LINES);

vector<rope<wchar_t>> text(MAX_LINES);
vector<size_t> len(MAX_LINES, 0); //lenght of each line


int y = 0, x = 0;
// offset in y axis of text and screen
int ofy = 0, ry;

int maxy = 0, maxx = 0; // to store the maximum rows and columns
uint8_t i;
wchar_t ch;
char filename[FILENAME_MAX];
wchar_t s[2]; // tmp array since there are only functions for wchar_t*

// output file
FILE *fo;

// indx: tmp for lenght of line
// curnum: total lines
int indx = 0, curnum = 0, mx = -1;

int main(int argc, char *argv[])
{
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
		FILE *fp;
		fp = fopen(argv[1], "r");
		if (fp == NULL) {
			perror("fopen");
			return -2; // ENOENT
		}
		while ((ch = fgetwc(fp)) != EOF) {
			text[curnum].push_back(ch);
			s[0] = ch;
			if (mx != 0 && indx < maxx)
				waddnwstr(text_win, s, 1);
			if (ch == '\n') {
				len[y++] = indx - 1;
				if (mx != 0 && y == maxy - 1)
					mx = 0;
				indx = 0;
				++curnum;
			}
			++indx;
		}
		fclose(fp);
	}

	mx = -1;
	wmove(text_win, 0, 0);
//	goto stop; /*

	while (1) {
		getyx(text_win, y, x);
		ry = y + ofy;
		if (x > (int)len[ry])
			wmove(text_win, y, len[ry]);
		wget_wch(text_win, (wint_t*)s);
		switch (s[0]) {
		case KEY_DOWN:
			if (ry > curnum) // do not scroll indefinetly
				break;
			if (y == (maxy - 1) && ry < curnum) {
				ofy++;
				wscrl(text_win, 1);
				wscrl(ln_win, 1);
				mvwprintw(ln_win, maxy - 1, 0, "%2d", ry + 1);
				wrefresh(ln_win);
				wmove(text_win, y, 0);
				for (const auto &c : text[ry]) {
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

		case KEY_UP:
			if (y == 0 && ofy != 0) {
				wscrl(text_win, -1); // scroll up
				wscrl(ln_win, -1);
				mvwprintw(ln_win, 0, 0, "%2d", ry);
				wrefresh(ln_win);
				--ofy;
				wmove(text_win, 0, 0);
				for (const auto &c : text[ry]) {
					if (getcurx(text_win) < maxx - 1) {
						s[0] = c;
						waddnwstr(text_win, s, 1);
					}
				}
				wmove(text_win, 0, x);
			} if (len[ry] > len[ry - 1]) {
				wmove(text_win, y - 1, len[ry + 1]);
				mx = getcurx(text_win);
			} else {
				wmove(text_win, y - 1, x);
			}
			break;

		case KEY_LEFT:
			if (x > 0)
				wmove(text_win, y, x - 1);
			else
				wmove(text_win, y - 1, len[ry - 1]);
			break;

		case KEY_RIGHT:
			if ((size_t)x < len[ry])
				wmove(text_win, y, x + 1);
			else if (ry < curnum)
				wmove(text_win, y + 1, 0);
			break;

		case KEY_BACKSPACE:
			mvwdelch(text_win, y, x - 1);
			text[ry].erase(x - 1, 1);
			len[ry]--;
			break;

		case KEY_DC:
			wdelch(text_win);
			text[ry].erase(x, 1);
			len[ry]--;
			break;

		case KEY_ENTER:
			wclrtoeol(text_win);                    
			wmove(text_win, y + 1, 0);
			text[ry].insert(x, '\n');
			if ((size_t)x != len[ry]) {
				len[ry] -= len[ry] - x;
				for (int i = 0; i < (int)len[ry]; ++i) {
					//s[0] =
					wprintw(text_win, "%c", (char)text[ry][i]);
				}
			}
			break;

		case ctrl('A'):
			wmove(text_win, y, 0);
			break;

		case ctrl('E'):
			getmaxyx(text_win, maxy, maxx);
			--maxx;
			wmove(text_win, y, maxx);
			while ((winch(text_win) & A_CHARTEXT) == ' ')
				wmove(text_win, y, --maxx);
			wmove(text_win, y, ++maxx);
			break;

		case ctrl('S'):
			i = 0;
			if (strlen(filename) != 0) {
				fo = fopen(filename, "w");
				for (i = 0; i < curnum; ++i)
					for (const wchar_t &c : text[i])
						fputwc(c, fo);
			} else {
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
					print2header(header_win, maxx, "ERROR");
					wmove(text_win, y, x);
					break;
				}
				fo = fopen((char*)filename, "w");

				while (text[i++].size() != 0) {
					//fputws(text[i].c_str(), fo);
					break;
				}
			}

			fclose(fo);

			print_header(header_win, maxx);
			print2header(header_win, maxx, "Saved");
			wmove(text_win, y, x);
			break;

		case ctrl('C'):
			goto stop;

		default:
			wins_nwstr(text_win, s, 1);
			wmove(text_win, y, x + 1);
			text[ry].insert(x, s[0]);
			len[ry]++;
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
