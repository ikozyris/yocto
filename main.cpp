#include "headers.h"
#include "utils/init.cpp"
#include "keybindings.h"
#include "utils/sizes.cpp"

#define MAX_LINES       1000

using namespace std;
using namespace __gnu_cxx;

//vector<crope> text(MAX_LINES);
vector<rope<wchar_t>> text(MAX_LINES);
rope<size_t> len(MAX_LINES);
rope<size_t>::iterator it;

short y = 0, x = 0;
// offset in y axis of text and screen
long int ofy = 0;
size_t ry;
short prevy;
int maxy = 0, maxx = 0; // to store the maximum rows and columns
uint8_t i;
wchar_t ch;
char filename[FILENAME_MAX];
wchar_t s[CCHARW_MAX]; // tmp array since there are only functions for wchar_t*

// file pointers input/output
FILE *fi;
FILE *fo;

// indx: tmp for lenght of line
// curnum: total lines
size_t indx = 0, curnum = 0;
signed int mx = -1;

int main(int argc, char *argv[])
{
	// UTF-8
	setlocale(LC_ALL, "");
	setlocale(LC_NUMERIC,"C");

	bzero(s, CCHARW_MAX);
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
				if (curnum >= text.capacity()) {
					text.resize(text.capacity() * 2);
				}
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
		if (x > (int)len[ry]) // hack
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
				mvwprintw(ln_win, maxy - 1, 0, "%3ld", ry + 2);
				wrefresh(ln_win);
				wmove(text_win, y, 0);
				for (const wchar_t &c : text[ry + 1]) {
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
				mvwprintw(ln_win, 0, 0, "%3ld", ry);
				--ofy;
				wmove(text_win, 0, 0);
				for (const wchar_t &c : text[ry - 1]) {
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
			it = len.mutable_begin();
			if (x != 0) {
				mvwdelch(text_win, y, x - 1);
				text[ry].erase(x - 1, 1);
				//*(it + ry)--; // see comment on line ~290
				it += ry;
				*it = *it + 1;
			} else { // delete previous line's \n
				text[ry - 1].erase(len[ry - 1], 1);
				*(it + ry + 1) = len[ry] + len[ry + 1];
				len.erase(ry, 1);
				print_text(text, text_win, maxx, maxy);
				--curnum;
			}
			wmove(text_win, y, x - 1);
			break;

		case DELETE:
			it = len.mutable_begin();
			if (x != 0) {
				wdelch(text_win);
				text[ry].erase(x - 1, 1);
				//*(it + ry)--;
				it += ry;
				*it = *it + 1;
			} else { // delete previous line's \n
				text[ry].erase(len[ry], 1);
				len.erase(ry, 1);
				print_text(text, text_win, maxx, maxy);
				--curnum;
				wmove(text_win, y, x);
			}
			break;

		case ENTER:
			text[ry].insert(x, '\n');
			len.insert(x, (size_t)0);
			prevy = y;
			++curnum;
			wclear(text_win);

			// HACK: print text again
			print_text(text, text_win, maxx, maxy);
			wmove(text_win, prevy + 1, x);
			wrefresh(text_win);
			break;

		case HOME:
			wmove(text_win, y, 0);
			break;

		case END:
			wmove(text_win, y, len[ry] + 1);
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
				if (wgetnstr(header_win, filename, FILENAME_MAX) == ERR ||
				    strlen(filename) == 0) {
					print_header(header_win, maxx);
					print2header(header_win, maxx, "ERROR", 1);
					wmove(text_win, y, x);
					break;
				}
			}
			fo = fopen(filename, "w");
			for (i = 0; i < curnum; ++i) {
				for (const wchar_t &c : text[i]) {
					fputwc(c, fo);
					if (i > maxx)
						break;
				}
			}
			fclose(fo);

			print_header(header_win, maxx);
			print2header(header_win, maxx, "Saved", 1);
			wmove(text_win, y, x);
			break;

		case INFO:
			char tmp[42];
			if (strlen(filename) != 0) {
				struct stat stat_buf;
				if (stat(filename, &stat_buf) == 0) {
					bzero(tmp, 42);
					strcat(tmp, itoa(curnum));
					strcat(tmp, " lines");
					print2header(header_win, maxx, hrsize(stat_buf.st_size), 3);
					print2header(header_win, maxx, tmp, 1);
				}
			}
			bzero(tmp, 42);
			strcat(tmp, "y: ");
			strcat(tmp, itoa(ry + 1));
			strcat(tmp, " x: ");
			strcat(tmp, itoa(x + 1));
			print2header(header_win, maxx, tmp, 2);
			wmove(text_win, y, x);
			break;

		case REFRESH:
			print_header(header_win, maxx);
			print_text(text, text_win, maxx, maxy);
			init_lines(ln_win, maxy);
			ofy = 0;
			wmove(text_win, 0, 0);
			break;

		case EXIT:
			goto stop;

		default:
			wins_nwstr(text_win, s, 1);
			wmove(text_win, y, text_win->_curx + 1);

			text[ry].insert(x, s[0]);
			it = len.mutable_begin();
			//*(it + ry)++; // this does not work 
			it += ry;
			*it = *it + 1;
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
