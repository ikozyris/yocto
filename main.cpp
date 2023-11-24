#include "utils/init.c"
#include "headers/keybindings.h"
#include "utils/sizes.c"

int main(int argc, char *argv[])
{
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

	FILE *fo;
	if (argc > 1) {
		strcpy(filename, argv[1]);
		FILE *fi = fopen(filename, "r");
		if (fi == NULL) {
			fo = fopen(filename, "w");
			print2header("New file", 1);
			goto read;
		}
		read_getc(fi);
		//read_fgets(fi);
		//read_fread(fi); // MUCH faster
		fclose(fi);
		print_text();
	}

//	sleep(5);
	wmove(text_win, 0, 0);
//	goto stop; /*
read:
	while (1) {
		getyx(text_win, y, x);
		ry = y + ofy; // calculate once
		if (x > text[ry].length) // if out of bounds: move (to avoid bugs)
			wmove(text_win, y, text[ry].length);
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
				print_line_no_nl(ry + 1);
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
				print_line(ry - 1);
				wmove(text_win, 0, x);
				wrefresh(ln_win);
			} else {
				wmove(text_win, y - 1, x);
			}
			break;

		case LEFT:
			if (x > 0)
				wmove(text_win, y, x - 1);
			else if (y > 0)
				wmove(text_win, y - 1, text[ry - 1].length);
			break;

		case RIGHT:
			if ((size_t)x < text[ry].length)
				wmove(text_win, y, x + 1);
			else if (ry < curnum)
				wmove(text_win, y + 1, 0);
			break;

		case BACKSPACE:
			if (x != 0) {
				mvwdelch(text_win, y, x - 1);
				eras(text[ry], x - 1);
			} else if (y != 0){ // delete previous line's \n
				eras(text[ry], text[ry].length);
				print_text();
				--curnum;
			}
			wmove(text_win, y, x - 1);
			break;

		case DELETE:
			if (x != 0) {
				wdelch(text_win);
				eras(text[ry], x - 1);
			} else { // delete previous line's \n
				eras(text[ry], text[ry].length);
				print_text();
				--curnum;
				wmove(text_win, y, x);
			}
			break;

		case ENTER:
			it = text.begin();
			it += ry;
			static gap_buf a;
			init(a);
			resize(a, 20);
			insert(a, 0, L'\n');
			text.insert(it, a);
			prevy = y;
			++curnum;

			// print text again (or find a better way)
			print_text();
			wmove(text_win, prevy + 1, 0);
			//wrefresh(text_win);
			break;

		case HOME:
			wmove(text_win, y, 0);
			break;

		case END:
			wmove(text_win, y, text[ry].length);
			break;

		case SAVE:
			if (strlen(filename) == 0) {
				print_header();
				print2header("Enter filename: ", 1);
				wmove(header_win, 0, 16);
				//wscanw(header_win, "%s", filename);

				echo();
				if (wgetnstr(header_win, filename, FILENAME_MAX) == ERR ||
				    strlen(filename) == 0) {
					print_header();
					print2header("ERROR", 1);
					wmove(text_win, y, x);
					break;
				}
			}
			noecho();
			fo = fopen(filename, "w");
			for (unsigned char i = 0; i <= curnum; ++i)
#if defined(UNICODE)
				fputws(text[i].buffer, fo);
#else
				fputs(data(text[i]), fo);
#endif
			fclose(fo);

			print_header();
			print2header("Saved", 1);
			wmove(text_win, y, x);
			break;

		case 27: // alt - i
			wtimeout(text_win, 1000);
			if (!(wgetch(text_win) == INFO))
				break;
			wtimeout(text_win, -42);
			char tmp[21];
			bzero(tmp, 21);
			if (strlen(filename) != 0) {
				struct stat stat_buf;
				if (stat(filename, &stat_buf) == 0) {
					sprintf(tmp, "%s lines", itoa(curnum));
					print2header(hrsize(stat_buf.st_size), 3);
					print2header(tmp, 1);
				}
			}
			sprintf(tmp, "y: %u x: %u", ry, x);
			print2header(tmp, 2);
			wmove(text_win, y, x);
			break;

		case KEY_RESIZE:
		case REFRESH:
			getmaxyx(text_win, maxy, maxx);
			print_header();
			print_text();
			print_lines();
			ofy = 0;
			wrefresh(text_win);
			wrefresh(ln_win);
			wrefresh(header_win);
			wmove(text_win, 0, 0);
			break;

		case EXIT:
			goto stop;

		case KEY_TAB:
			winsch(text_win, '\t');
			wmove(text_win, y, x + 8);
			insert(text[ry], x, '\t');
			break;

		default:
			if (s[0] < 32)
				break;
			wins_nwstr(text_win, s, 1);
			wmove(text_win, y, text_win->_curx + 1);

			insert(text[ry], x, s[0]);
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
