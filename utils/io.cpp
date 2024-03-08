#include "init.c"

// pos: (1 left) (3 right) (else center)
void print2header(const char *msg, unsigned char pos)
{
	if (pos == 3) {
		mvwprintw(header_win, 0, maxx - strlen(msg), "%s", msg);
	} else if (pos == 1) {
		mvwprintw(header_win, 0, 0, "%s", msg);
	} else {
		unsigned char hmx = maxx / 2;
		// 20-width spaces + 0
		mvwprintw(header_win, 0, hmx - 10, "                    ");
		mvwprintw(header_win, 0, hmx - strlen(msg) / 2, "%s", msg);
	}
	wrefresh(header_win);
}

// Ask for input from header
char *input_header(const char *q)
{
	clear_header();
	wmove(header_win, 0, 0);
	wprintw(header_win, "%s", q);
	wmove(header_win, 0, strlen(q));
	echo();
	char *tmp = (char*)malloc(sizeof(char) * 128);
	if (wgetnstr(header_win, tmp, 128) == ERR) { [[unlikely]]
		reset_header();
		print2header("ERROR", 1);
		wmove(text_win, y, x);
	} if (strlen(tmp) <= 0) {
		tmp = 0;
	}
	noecho();
	return tmp;
}

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

#if defined(UNICODE)
#define print_line(a) waddnwstr(text_win, data((a), 0, maxx), min((a).length, maxx))
#define print_line_no_nl(a) waddnwstr(text_win, data((a), 0, maxx), min((a).length - 1, maxx))
#else
#define print_line(a) waddnstr(text_win, data((a), 0, maxx), min((a).length, maxx))
#define print_line_no_nl(a) waddnstr(text_win, data((a), 0, maxx), min((a).length - 1, maxx))
#endif

void print_text()
{
	std::list<gap_buf>::iterator iter = text.begin();
	wclear(text_win);
	for (unsigned char i = 0; i < min(txt_cpt, maxy - 1); ++i, ++iter)
		print_line((*iter));
	print_line_no_nl((*iter));
}

inline void read_getc(FILE *fi)
{
#if defined(UNICODE)
	while ((ch = getwc_unlocked(fi)) != EOF) {
#else
	while ((ch = getc_unlocked(fi)) != EOF) {
#endif
		apnd_c(*it, ch);
		if (ch == '\n')  { [[unlikely]]
			++curnum;
			if (curnum >= txt_cpt) { [[unlikely]]
				txt_cpt = text.size() * 2;
				text.resize(txt_cpt);
			}
			++it;
		}
	}
}
#define sz 256

inline void read_fgets(FILE *fi)
{
	tp tmp[sz];
#if defined(UNICODE)
	while ((fgetws_unlocked(tmp, sz, fi)))
#else
	while ((fgets_unlocked(tmp, sz, fi)))
#endif 
{
		apnd_s(*it, tmp);
		if (tmp[sz - 1] == 0) { [[unlikely]]
			++curnum;
			++it;
		}
	}
}

inline void read_fread(FILE *fi)
{
	tp tmp[sz];
	size_t a = 0;
	while ((a = fread(tmp, sizeof(tmp[0]), sz, fi))) {
		apnd_s(*it, tmp, a);
	}
}
