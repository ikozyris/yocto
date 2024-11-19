#include "utils/key_func.cpp"
#include "headers/keybindings.h"

int main(int argc, char *argv[])
{
	if (argc > 1 && (strcmp(argv[1], "-h") == 0 ||
	strcmp(argv[1], "--help") == 0)) {
		puts(name);
		puts("A simple, compact and fast text editor.\n"
		"Source code: https://github.com/ikozyris/yocto\n"
		"Wiki: https://github.com/ikozyris/yocto/wiki\n"
		"License: GNU GPL v3\n");
		puts("Usage:\n"
		"       --help, -h          Show this help\n"
		"<file>                     Open file and allow edits\n"
		"-                          Ask for file name on save\n\n"
		"Keybindings:\n"
		"Save:                      Ctrl-S\n"
		"Exit:                      Ctrl-C\n"
		"Go to start of line:       Ctrl-A\n"
		"Go to end of line:         Ctrl-E\n"
		"Enter built-in terminal:   Alt-C\n"
		"Show debbuging info:       Alt-I (also command stats in built-in terminal)\n\n"
		"Built-in terminal commands:\n"
		"scroll                     Scroll to line\n"
		"usage                      Show RAM usage and PID of yocto\n"
		"run                        Execute a command\n"
		"help                       List commands\n"
		"fixgap                     Benchmark move gap to end and back");
		return 0;
	}
	it = text.begin();

	init_curses();
	getmaxyx(stdscr, maxy, maxx);

	// initialize windows
	init_header();
	init_lines();
	init_text();

	getmaxyx(text_win, maxy, maxx);
	wnoutrefresh(ln_win);
	wnoutrefresh(header_win);
	doupdate();

read:
	if (argc > 1) {
		filename = (char*)malloc(sizeof(char) * 128);
		strcpy(filename, argv[1]);
		FILE *fi = fopen(filename, "r");
		if (fi == NULL) {
			print2header("New file", 1);
			goto loop;
		}
#ifdef HIGHLIGHT
		eligible = isc(argv[1]); // syntax highlighting
#endif
		read_fread(fi);
		print_text(0);
		fclose(fi);
	}
	wmove(text_win, 0, 0);
	// all functions think there is a newline at EOL, emulate it
	if (at(*it, it->len) != '\n')
		apnd_c(*it, 0);
	it = text.begin();
loop:
	while (1) {
		getyx(text_win, y, x);
		ry = y + ofy;
		// if out of bounds: move (to avoid bugs) & TODO: simplify
		if (x > min(it->len - 1 - ofx, maxx))
			wmove(text_win, y, x = min(it->len - ofx - 1, maxx));
		rx = x + ofx;
		mv_curs(*it, rx);

#ifndef RELEASE
		stats();
#endif
#ifdef DEBUG
		print_text(y);
		wmove(text_win, y, x);
#endif
	//goto stop;
		wget_wch(text_win, (wint_t*)s);
		switch (s[0]) {
		case DOWN:
			if (ry >= curnum) // do not scroll indefinetly
				break;
			if (!cut.empty()) // revert cut
				mvprint_line(y, 0, *it, 0, 0);
			cut.clear();
			ofx = 0; // invalidated
			if (y == maxy - 1 && ry < curnum)
				scrolldown();
			else {
				++it;
				ofx = calc_offset_dis(x, *it);
				wmove(text_win, y + 1, flag);
			}
			break;

		case UP:
			if (!cut.empty()) // revert cut
				mvprint_line(y, 0, *it, 0, 0);
			if (y == 0 && ofy != 0)
				scrollup();
			else if (y != 0) {
				--it;
				ofx = calc_offset_dis(x, *it);
				wmove(text_win, y - 1, flag);
			}
			cut.clear();
			break;

		case LEFT:
			left();
			break;

		case KEY_SLEFT:
			prevword();
			break;

		case RIGHT:
			right();
			break;

		case KEY_SRIGHT:
			nextword();
			break;

		case BACKSPACE:
			if (x != 0) {
				eras(*it);
				if (it->buffer[it->gps] == '\t') {
					wmove(text_win, y, x - 1);
					ofx += prevdchar();
					x = getcurx(text_win);
					wmove(text_win, y, 0);
					print_line(*it);
					wclrtoeol(text_win);
					wmove(text_win, y, x);
				} else
					mvwdelch(text_win, y, x - 1);
			} else if (y != 0) { // x = 0; merge lines
				list<gap_buf>::iterator curln = it;
				--it;
				mv_curs(*it, it->len); // delete \n
				it->gpe = it->cpt - 1;
				unsigned tmp = --(it->len);
				data(*curln, 0, curln->len);
				apnd_s(*it, lnbuf, curln->len); // merge
				text.erase(curln); // delete actual line
				--curnum;
				print_text(y - 1);
				wmove(text_win, y - 1, tmp);
			}
			break;

		case DELETE:
			if (rx + 1u > it->len)
				break;
			if (it->buffer[it->gpe + 1u] == '\n') { // similar to backspace
				list<gap_buf>::iterator curln = it; // current line
				curln->gpe = curln->cpt - 1; // delete newline
				curln->len--;
				++it; // next line
				data(*it, 0, it->len);
				apnd_s(*curln, lnbuf, it->len);
				text.erase(it);
				it = curln;
				--curnum;
				print_text(y);
				wmove(text_win, y, x);
			} else {
				wdelch(text_win);
				// or mblen(it->buffer + it->gpe + 1, 3);
				unsigned len = it->buffer[it->gpe + 1] < 0 ? 2 : 1;
				mveras(*it, rx + len);
				ofx += len - 1;
			}
			break;

		case ENTER:
			enter();
			break;

		case HOME:
			sol();
			break;

		case END:
			eol();
			break;

		case SAVE:
			save();
			break;

		case 27: // ALT or ESC
			wtimeout(text_win, 1000);
			ch = wgetch(text_win);
			if (ch == INFO)
				stats();
			else if (ch == CMD)
				command();
			else if (ch == SWITCH) { // switch file
				argc = 2;
				argv[1] = input_header("File to open: ");
				list<gap_buf>::iterator iter;
				unsigned i;
				for (iter = text.begin(), i = 0; iter != text.end() && i <= curnum; ++iter, ++i) {
					iter->len = iter->gps = 0;
					iter->gpe = iter->cpt;
				}
				curnum = 0u;
				it = text.begin();
				wclear(text_win);
				goto read;
			}
			wtimeout(text_win, -1);
			wmove(text_win, y, x);
			break;

		case REFRESH:
			reset_view();
			break;

		case KEY_RESIZE:
			endwin();
			refresh();
			getmaxyx(stdscr, maxy, maxx);
			reset_header();
			init_lines();
			print_lines();
			delwin(text_win);
			init_text();
			wnoutrefresh(ln_win);
			wnoutrefresh(header_win);
			doupdate();
			getmaxyx(text_win, maxy, maxx);
			reset_view();
			break;

		case EXIT:
			goto stop;

		case KEY_TAB:
			winsnstr(text_win, "        ", 8 - x % 8);
			wmove(text_win, y, x + 8 - x % 8);
			insert_c(*it, rx, '\t');
			break;

		default:
			if (s[0] > 0 && s[0] < 32) // not a character
				break;
			if (x == maxx - 1) { // cut line
				cut.push_back({maxx - 1, ofx});
				clearline;
				mvprint_line(y, 1, *it, ofx, 0);
				wmove(text_win, y, x = 0);
			} if (at(*it, rx) == '\t') {
				waddnwstr(text_win, s, 1);
				if (x % 8 >= 7)
					winsch(text_win, '\t');
			} else {
				wins_nwstr(text_win, s, 1);
				wmove(text_win, y, x + 1);
			}
			len = wcstombs(s2, s, 4);
			insert_s(*it, rx, s2, len);
			if (len > 1)
				ofx += len - 1; // UTF-8 character
			break;
		}
	}
stop:
	free(lnbuf);
	delwin(text_win);
	delwin(ln_win);
	delwin(header_win);
	endwin();
	return 0;
}
