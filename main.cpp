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
		"<file> --read-only, -ro    Just read, like a pager\n"
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

		if (argc > 2 && (strcmp(argv[2], "-ro") == 0 ||
				strcmp(argv[2], "--read-only") == 0)) {
			read_fread_sl(fi);
			printed = print_text_w(0);
			print2header("Read-Only", 3);
			goto ro;
		} else {
			//read_fgets(fi);
			read_fread(fi);
			print_text(0);
		}
		fclose(fi);
	}
	wmove(text_win, 0, 0);

	it = text.begin();
loop:
	//goto stop;
	while (1) {
		getyx(text_win, y, x);
		ry = y + ofy;
		// if out of bounds: move (to avoid bugs)
		if (x >= min(ry < curnum ? (it->len - 1 - ofx) : (it->len - ofx), maxx))
			wmove(text_win, y, min((ry != curnum ? it->len - ofx - 1 : it->len - ofx), maxx));
		getyx(text_win, y, x);
		rx = x + ofx;
		mv_curs(*it, rx);

#ifdef DEBUG
		print_text(); // debug only
		char tmp[80];
		snprintf(tmp, 79, "st %u | end %u | cpt %u | len %u | gapLen %u | x %u  ",
			it->gps, it->gpe, it->cpt, it->len, gaplen(*it), x);
		print2header(tmp, 1);
		wmove(text_win, y, x);
#endif
		wget_wch(text_win, (wint_t*)s);
		switch (s[0]) {
		case DOWN:
			if (ry >= curnum) // do not scroll indefinetly
				break;
			++it;
			ofx = 0; // invalidated
			if (y == (maxy - 1) && ry < curnum) {
				ofy++;
				wscrl(text_win, 1);
				wscrl(ln_win, 1);
				mvwprintw(ln_win, maxy - 1, 0, "%3u", ry + 2);
				wrefresh(ln_win);
				wmove(text_win, y, 0);
				print_line(*it);
#ifdef HIGHLIGHT
				wmove(text_win, y, 0);
				apply(y);
#endif
				wmove(text_win, y, x);
			} else
				wmove(text_win, y + 1, x);
			break;

		case UP:
			if (y == 0 && ofy != 0) {
				wscrl(text_win, -1); // scroll up
				wscrl(ln_win, -1);
				mvwprintw(ln_win, 0, 0, "%3u", ry);
				--ofy;
				wmove(text_win, 0, 0);
				--it;
				print_line(*it);
#ifdef HIGHLIGHT
				wmove(text_win, y, 0);
				apply(y);
#endif
				wmove(text_win, 0, x);
				wrefresh(ln_win);
				ofx = 0;
			} else if (y != 0) {
				wmove(text_win, y - 1, x);
				--it;
				ofx = 0;
			}
			break;

		case LEFT:
			if (x == 0 && rx >= maxx - 1) { // line has been wrapped
				wmove(text_win, y, 0);
				wclrtoeol(text_win);
				print_line(*it);
#ifdef HIGHLIGHT
				wmove(text_win, y, 0);
				apply(y);
#endif
				ofx -= maxx - 1;
				wmove(text_win, y, maxx - 1);
			} else if (x > 0)
				wmove(text_win, y, x - 1);
			else if (y > 0) { // x = 0
				if (ofx > 0) // revert wrap
					print_line(*it);
				--it;
				wmove(text_win, y - 1, min(it->len, maxx) - 1);
				ofx = 0;
			}
			break;

		case RIGHT:
			if (x == maxx - 1 && x < it->len - 1) {
				wmove(text_win, y, 0);
				wclrtoeol(text_win);
				// printline() with custom start
				data2(*it, ofx + maxx - 1, ofx + maxx * 2);
				waddnstr(text_win, lnbuf, maxx - 1);

				ofx += maxx - 1;
				wmove(text_win, y, 0);
			} else if (ry != curnum ? rx < it->len - 1 : rx < it->len)
				wmove(text_win, y, x + 1);
			else if (ry < curnum) {
				wmove(text_win, y, 0);
				if (ofx > 0)
					print_line(*it);
				wmove(text_win, y + 1, 0);
				++it;
				ofx = 0;
			}
			break;

		case BACKSPACE:
			if (x != 0) {
				eras(*it);
				mvwdelch(text_win, y, x - 1);
			} else if (y != 0) { // delete previous line's \n
				std::list<gap_buf>::iterator tmp = it;
				--it;
				mv_curs(*it, it->len);
				eras(*it);
				data(*tmp, 0, tmp->len + 1);
				apnd_s(*it, lnbuf, tmp->len + 1);
				text.erase(tmp);
				--curnum;
				print_text(y - 1);
				wmove(text_win, y - 1, it->len - 1);
			}
			break;

		case DELETE:
			wdelch(text_win);
			mv_curs(*it, rx + 1);
			if (rx + (unsigned)1 <= it->len)
				mveras(*it, rx + 1);
			break;

		case ENTER:
			enter();
			break;

		case HOME:
			wmove(text_win, y, 0);
			x = 0; rx = ofx;
			if (ofx >= it->len - maxx) { // line has been wrapped
				wclrtoeol(text_win);
				print_line(*it);
#ifdef HIGHLIGHT
				wmove(text_win, y, 0);
				apply(y);
#endif
				ofx = 0;
				wmove(text_win, y, 0);
			} 
			break;

		case END:
			wmove(text_win, y, ofx + (ry != curnum ? it->len - 1 : it->len));
			if (it->len >= maxx + ofx) {
				wmove(text_win, y, 0);
				wclrtoeol(text_win);
				// TODO: use actual printline()
				data2(*it, it->len - maxx, it->len);
				waddnstr(text_win, lnbuf, maxx - 1);

				ofx += it->len - maxx;
			}
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
				strcpy(argv[1], input_header("File to open: "));
				std::list<gap_buf>::iterator iter;
				unsigned i;
				for (iter = text.begin(), i = 0; iter != text.end() && i < curnum; ++iter, ++i) {
					iter->len = iter->gps = 0;
					iter->gpe = iter->cpt;
				}
				curnum = 0;
				it = text.begin();
				wclear(text_win);
				goto read;
			} else if (ch == OFF) { // calculate x offset
				x = sizeofline(y);
				print2header(itoa(x), 1);
				ofx = (long)it->len - (long)x;
				print2header(itoa(ofx), 2);
			}
			wtimeout(text_win, -1);
			wmove(text_win, y, x);
			break;

		case KEY_RESIZE:
		case REFRESH:
			getmaxyx(text_win, maxy, maxx);
			ofy = 0;
			ofx = 0;
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
			if (x == maxx - 1) {
				ofx += maxx - 1;
				wmove(text_win, y, 0);
				wclrtoeol(text_win);
				x = 0;
				rx = x + ofx;
			}
			wins_nwstr(text_win, s, 1);
			wmove(text_win, y, x + 1);
			len = wcstombs(s2, s, 4);
			insert_s(*it, rx, s2, len);
			if (len > 1)
				ofx += len - 1; // Unicode character
			break;
		}
	}
	//*/
ro:
	wclear(ln_win);
	do {
		switch (ch) {
		case UP:
			if (ppi >= previndx || ppi >= buf_indx)
				ppi = 0;
			else
				ppi = previndx;
			printed = print_text_w(ppi);
			if (buf_indx > printed)
				previndx = buf_indx - printed;
			else
				previndx = 0;
			break;
		case DOWN:
			previndx = buf_indx - printed;
			printed = print_text_w(buf_indx);
			break;
		case EXIT:
			goto stop;
		}
#ifdef HIGHLIGHT
		for (unsigned i = 0; i < maxy - 1; ++i) {
			wmove(text_win, i, 0);
			apply(i);
		}
#endif
		mvwprintw(ln_win, 1, 0, "%3u", buf_indx);
		mvwprintw(ln_win, 3, 0, "%3u", printed);
		mvwprintw(ln_win, 5, 0, "%3u", previndx);
		mvwprintw(ln_win, 7, 0, "%3u", ppi);
		wrefresh(ln_win);

	} while ((ch = wgetch(text_win)));
stop:
	free(lnbuf);
	delwin(text_win);
	delwin(ln_win);
	delwin(header_win);
	endwin();
	return 0;
}
