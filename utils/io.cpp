#include "highlight.c"

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
	} if (strlen(tmp) <= 0)
		bzero(tmp, 128);
	noecho();
	return tmp;
}

#define print_line(a)       {char *t = data((a), 0, maxx); waddnstr(text_win, t, maxx); free(t);}
#define print_line_no_nl(a) {char *t = data((a), 0, maxx-1); waddnstr(text_win, t, min((a).len-1, maxx-1)); free(t);}

void print_text()
{
	std::list<gap_buf>::iterator iter = text.begin();
	std::advance(iter, ofy);
	wclear(text_win);
	unsigned ty = 0;
	for (unsigned char i = ofy; i < ofy + min(txt_cpt, maxy ) && iter != text.end(); ++i, ++iter) {
		print_line_no_nl((*iter));
#ifdef HIGHLIGHT
		wmove(text_win, ty, 0);
		apply(ty);
#endif
		wmove(text_win, ++ty, 0);
	}
}

// print text with line wrapping
unsigned print_text_w(unsigned start)
{
	wmove(text_win, 0, 0);
	buf_indx = start;
	while ((unsigned)getcury(text_win) < maxy-1 && buf_indx < it->len)
		waddch(text_win, it->buffer[buf_indx++]);
	wclrtobot(text_win);
	return buf_indx - start; // how many characters were printed
}

void save()
{
	if (!filename)
		filename = (char*)malloc(128);
	if (strlen(filename) <= 0)
		filename = (char*)input_header("Enter filename: ");
	FILE *fo = fopen(filename, "w");
	unsigned i;
	std::list<gap_buf>::iterator iter;
	for (iter = text.begin(), i = 0; iter != text.end() && i < curnum; ++iter, ++i) {
		char *tmp = data((*iter), 0, iter->cpt);
		fputs(tmp, fo);
		free(tmp);
	}
	fclose(fo);

	reset_header();
	print2header("Saved", 1);
	wmove(text_win, y, x);
}

// For size see: https://github.com/ikozyris/yocto/wiki/Comments-on-optimizations#buffer-size-for-reading
#define SZ 524288 // 512 KB

void read_fgets(FILE *fi)
{
	char tmp[SZ];
	while ((fgets_unlocked(tmp, SZ - 2, fi))) {
		apnd_s(*it, tmp);
		if (tmp[SZ - 3] == 0) { [[unlikely]]
			++curnum;
			++it;
		}
	}
}

inline void ins_b(char ch)
{
	it->buffer[it->len++] = ch;
	if (it->len >= it->cpt) { [[likely]]
		it->cpt *= 2;
		it->buffer = (char*)realloc(it->buffer, it->cpt);
	} if (ch == '\n')  { [[unlikely]]
		it->gps = it->len;
		it->gpe = it->cpt;
		if (++curnum >= txt_cpt) { [[unlikely]]
			txt_cpt *= 2;
			text.resize(txt_cpt);
		}
		++it;
	}
}

// read in single line
void read_fread_sl(FILE *fi)
{
	char tmp[SZ];
	unsigned a = 0; // bytes read
	while ((a = fread(tmp, sizeof(tmp[0]), SZ, fi)))
		apnd_s(*it, tmp, a);
}

void read_fread(FILE *fi)
{
	char tmp[SZ];
	unsigned a;
	while ((a = fread(tmp, sizeof(tmp[0]), SZ, fi)))
		for (unsigned i = 0; i < a; ++i)
			ins_b(tmp[i]);
}
