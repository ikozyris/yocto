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
	// UTF-8
	setlocale(LC_ALL, "");
	setlocale(LC_NUMERIC,"C");

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

		eligible = isc(argv[1]); // syntax highlighting
		//read_fgets(fi);
		read_fread(fi);
		print_text(0);
		fclose(fi);
	}
	wmove(text_win, 0, 0);

	it = text.begin();
loop:
	while (1) {
		getyx(text_win, y, x);
		ry = y + ofy;
		// if out of bounds: move (to avoid bugs) & TODO: simplify
		if (x >= min(ry < curnum ? (it->len - 1 - ofx) : (it->len - ofx), maxx))
			wmove(text_win, y, min((ry != curnum ? it->len - ofx - 1 : it->len - ofx), maxx));
		getyx(text_win, y, x);
		rx = x + ofx;
		mv_curs(*it, rx);

#ifndef RELEASE
		stats();
#endif
#ifdef DEBUG
		print_text(y); // debug only
		wmove(text_win, y, x);
#endif
	//goto stop;
		wget_wch(text_win, (wint_t*)s);
		switch (s[0]) {
		case DOWN:
			if (ry >= curnum) // do not scroll indefinetly
				break;
			if (!wrap.empty()) { // revert wrap
				wmove(text_win, y, 0);
				print_line(*it);
			}
			++it;
			wrap.clear();
			ofx = 0; // invalidated
			if (y == (maxy - 1) && ry < curnum)
				scrolldown();
			else {
				wmove(text_win, y + 1, x);
				ofx = calc_offset_dis(x, 0, *it);
			}
			break;

		case UP:
			if (!wrap.empty()) { // revert wrap
				wmove(text_win, y, 0);
				print_line(*it);
			} if (y == 0 && ofy != 0)
				scrollup();
			else if (y != 0) {
				wmove(text_win, y - 1, x);
				--it; 
				ofx = calc_offset_dis(x, 0, *it);
			}
			wrap.clear();
			break;

		case LEFT:
			if (x == 0 && ofx == 0 && ofy > 0 && y == 0)
				scrollup();
			else
				left();
			break;

		case RIGHT:
			if (y == maxy - 1 && rx == it->len - 1) {
				++it;
				scrolldown();
			} else
				right();
			break;

		case BACKSPACE:
			if (x != 0) {
				eras(*it);
				if (it->buffer[it->gps] == '\t') {
					wmove(text_win, y, 0);
					print_line(*it);
					wclrtoeol(text_win);
					wmove(text_win, y, x - 8u);
					ofx += 7;
				} else
					mvwdelch(text_win, y, x - 1);
			} else if (y != 0) { // x = 0; merge lines
				std::list<gap_buf>::iterator tmpi = it;
				--it;
				mv_curs(*it, it->len); // delete \n
				eras(*it);
				unsigned temp = tmpi->len;
				data(*tmpi, 0, temp);
				apnd_s(*it, lnbuf, temp); // merge
				text.erase(tmpi); // delete actual line
				--curnum;
				print_text(y - 1u);
				wmove(text_win, y - 1, it->len - temp);
			}
			break;

		case DELETE:
			if (rx + 1u > it->len)
				break;
			if (it->buffer[it->gpe + 1u] == '\n') { // similar to backspace
				std::list<gap_buf>::iterator tmpi = it;
				it->gpe = it->cpt; // delete newline
				unsigned temp = it->len--;
				++it;
				data(*it, 0u, it->len);
				apnd_s(*tmpi, lnbuf, temp + 1u);
				text.erase(it);
				it = tmpi;
				--curnum;
				print_text(y);
				wmove(text_win, y, it->len - temp - 1u);
			} else {
				wdelch(text_win);
				mv_curs(*it, rx + 1u);
				mveras(*it, rx + 1u);
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
				std::list<gap_buf>::iterator iter;
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

		case KEY_RESIZE:
		case REFRESH:
			getmaxyx(text_win, maxy, maxx);
			ofy = ofx = 0;
			wrap.clear();
			reset_header();
			print_text(0);
			print_lines();
			wnoutrefresh(text_win);
			wnoutrefresh(ln_win);
			wnoutrefresh(header_win);
			doupdate();
			getmaxyx(text_win, maxy, maxx);
			wmove(text_win, 0, 0);
			it = text.begin();
			break;

		case EXIT:
			goto stop;

		// TODO: use real tab
		case KEY_TAB:
			winsnstr(text_win, "        ", 8);
			wmove(text_win, y, x + 8);
			insert_s(*it, x, "        ", 8);
			break;

		default:
			if (s[0] > 0 && s[0] < 32) // not a character
				break;
			if (x == maxx - 1) { // wrap line
				wrap.push_back(maxx - 1);
				ofx += maxx - 1;
				rx = ofx;
				x = 0;
				wmove(text_win, y, 0);
				wclrtoeol(text_win);
			}
			wins_nwstr(text_win, s, 1);
			wmove(text_win, y, x + 1);
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
