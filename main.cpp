#include "utils/key_func.cpp"
#include "headers/keybindings.h"

int main(int argc, char *argv[])
{
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
	wrefresh(ln_win);

	if (argc > 1) {
		filename = (char*)malloc(sizeof(char) * 128);
		strcpy(filename, argv[1]);
		FILE *fi = fopen(filename, "r");
		if (fi == NULL) {
			print2header("New file", 1);
			goto loop;
		}
		read_getc(fi);
		//read_fgets(fi);
		//read_fread(fi); // MUCH faster
		fclose(fi);
		print_text();
	}
	wmove(text_win, 0, 0);

	it = text.begin();
loop:
	//goto stop;
	while (1) {
		getyx(text_win, y, x);
		ry = y + ofy; // calculate offset
		if (x > min((ry != curnum ? it->len-1 : it->len), maxx)) // if out of bounds: move (to avoid bugs)
			wmove(text_win, y, min((ry != curnum ? it->len-1 : it->len), maxx));
		mv_curs(*it, x);

#ifdef DEBUG
		print_text(); // debug only
		char tmp[128];
		sprintf(tmp, "st %u | end %u | cpt %u | len %u | gapLen %u | x %u | currCh %d", 
			it->gps, it->gpe, it->cpt, it->len, it->gpe-it->gps, x, it->buffer[x-1]);
		clear_header();
		print2header(tmp, 1);
		wmove(text_win, y, x);
#endif
		wget_wch(text_win, (wint_t*)s);
		switch (s[0]) {
		case DOWN:
			if (ry >= curnum) // do not scroll indefinetly
				break;
			++it;
			if (y == (maxy - 1) && ry < curnum) {
				ofy++;
				wscrl(text_win, 1);
				wscrl(ln_win, 1);
				mvwprintw(ln_win, maxy - 1, 0, "%3d", ry + 2);
				wrefresh(ln_win);
				wmove(text_win, y, 0);
				print_line_no_nl(*it);
				wmove(text_win, y, x);
			} else {
				wmove(text_win, y + 1, x);
			}
			break;

		case UP:
			if (y == 0 && ofy != 0) {
				wscrl(text_win, -1); // scroll up
				wscrl(ln_win, -1);
				mvwprintw(ln_win, 0, 0, "%3d", ry);
				--ofy;
				wmove(text_win, 0, 0);
				--it; // why can't *(--it) work?
				print_line(*it);
				wmove(text_win, 0, x);
				wrefresh(ln_win);
			} else if (y != 0) {
				wmove(text_win, y - 1, x);
				--it;
			}
			break;

		case LEFT:
			if (x > 0)
				wmove(text_win, y, x - 1);
			else if (y > 0)
				wmove(text_win, y - 1, (--it)->len);
			break;

		case RIGHT:
			if (x < it->len)
				wmove(text_win, y, x + 1);
			else if (ry < curnum) {
				wmove(text_win, y + 1, 0);
				++it;
			}
			break;

		case BACKSPACE:
			if (x != 0) {
				mvwdelch(text_win, y, x - 1);
				eras(*it, x - 1);
			} else if (y != 0) { // delete previous line's \n
				eras(*it, it->len);
				print_text();
				--curnum;
			}
			break;

		case DELETE:
			wdelch(text_win);
			mv_curs(*it, x + 1);
			eras(*it, x);
			break;

		case ENTER:
			enter();
			break;

		case HOME:
			wmove(text_win, y, 0);
			break;

		case END:
			wmove(text_win, y, it->len);
			break;

		case SAVE:
			save();
			break;

		case 27: // ALT or ESC
			wtimeout(text_win, 1000);
			ch = wgetch(text_win);
			if (ch == INFO)
				info();
			else if (ch == CMD)
				command();
			wtimeout(text_win, -1);
			wmove(text_win, y, x);
			break;

		case KEY_RESIZE:
		case REFRESH:
			getmaxyx(text_win, maxy, maxx);
			reset_header();
			print_text();
			print_lines();
			ofy = 0;
			wrefresh(text_win);
			wrefresh(ln_win);
			wrefresh(header_win);
			wmove(text_win, 0, 0);
			it = text.begin();
			break;

		case EXIT:
			goto stop;

		// TODO: use real tab
		case KEY_TAB:
			winsnstr(text_win, "    ", 4);
			wmove(text_win, y, x + 4);
			insert_s(*it, x, "    ", 4);
			break;

		default:
			if (s[0] > 0 && s[0] < 32)
				break;
			wins_nwstr(text_win, s, 1);
			wmove(text_win, y, text_win->_curx + 1);
			len = wcstombs(s2, s, 4);
			insert_s(*it, x, s2, len);
			if (len > 1)
				ofx--; // Unicode character
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
