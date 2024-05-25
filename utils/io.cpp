#include "init.c"

// pos: (1 left) (3 right) (else center)
void print2header(const char *msg, unsigned char pos)
{
	if (pos == 3)
		mvwprintw(header_win, 0, maxx - strlen(msg), "%s", msg);
	else if (pos == 1)
		mvwprintw(header_win, 0, 0, "%s", msg);
	else {
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
		bzero(tmp, 128);
	}
	noecho();
	return tmp;
}

#define min(a, b) (a <= b ? a : b)
#define max(a, b) (a >= b ? a : b)

#define print_line(a) waddnstr(text_win, data((a), 0, maxx), min((a).len, maxx))
#define print_line_no_nl(a) waddnstr(text_win, data((a), 0, maxx), min((a).len - 1, maxx))

void print_text()
{
	std::list<gap_buf>::iterator iter = text.begin();
	std::advance(iter, ofy);
	wclear(text_win);
	for (unsigned char i = ofy; i < ofy + min(txt_cpt, maxy - 1); ++i, ++iter)
		print_line((*iter));
	if (curnum >= maxy)
		print_line_no_nl((*iter));
	else
		print_line((*iter));
}

// print text with line wrapping
unsigned print_text_w(unsigned start)
{
	wmove(text_win, 0, 0);
	buf_indx = start;
	while (getcury(text_win) < maxy-1 && buf_indx < it->len)
		waddch(text_win, it->buffer[buf_indx++]);
	wclrtobot(text_win);
	return buf_indx - start; // how many characters were printed
}

inline void read_getc(FILE *fi)
{
	while ((ch = getc_unlocked(fi)) != EOF) {
		it->buffer[it->len++] = ch;
		if (it->len >= it->cpt) { [[unlikely]]
			it->cpt *= 2;
			it->buffer = (char*)realloc(it->buffer, it->cpt);
		} if (ch == '\n')  { [[unlikely]]
			it->gps = it->len;
			it->gpe = it->cpt;
			if (++curnum >= txt_cpt) { [[unlikely]]
				txt_cpt = text.size() * 2;
				text.resize(txt_cpt);
			}
			++it;
		}
	}
}

#define sz 65536

inline void read_fgets(FILE *fi)
{
	char tmp[sz + 2];
	while ((fgets_unlocked(tmp, sz, fi))) {
		apnd_s(*it, tmp);
		if (tmp[sz - 1] == 0) { [[unlikely]]
			++curnum;
			++it;
		}
	}
}

inline void read_fread(FILE *fi)
{
	char tmp[sz];
	unsigned a = 0; // bytes read
	while ((a = fread(tmp, sizeof(tmp[0]), sz, fi))) {
		apnd_s(*it, tmp, a);
	}
}
