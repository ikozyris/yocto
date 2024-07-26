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

unsigned print_line(const gap_buf &buffer)
{
	if (buffer.len == 0)
		return 0;
	unsigned rlen = data(buffer, 0, min(buffer.len, maxx * 2));
	if (lnbuf[rlen - 1] == '\n')
		--rlen;
	for (unsigned i = 0; i < rlen && (unsigned)getcurx(text_win) < maxx - 1; ++i)
		waddnstr(text_win, lnbuf + i, 1);
	return rlen;
}

void print_text()
{
	std::list<gap_buf>::iterator iter = text.begin();
	std::advance(iter, ofy);
	wclear(text_win);
	for (unsigned ty = 0; ty < min(curnum + ofy + 1, maxy) && iter != text.end(); ++iter) {
		print_line(*iter);
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
	while ((unsigned)getcury(text_win) < maxy - 1 && buf_indx < it->len)
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
		data(*iter, 0, iter->len);
		fputs(lnbuf, fo);
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
	while ((fgets_unlocked(tmp, SZ, fi))) {
		apnd_s(*it, tmp);
		if (it->buffer[it->len - 1] == '\n') { [[unlikely]]
			if (++curnum >= txt_cpt) { [[unlikely]]
				txt_cpt *= 2;
				text.resize(txt_cpt);
			}
			++it;
		}
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

inline long whereis(const char *str,  char ch)
{
	const char *end = strchr(str, ch);
	if (end == 0)
		return -1;
	return end - str + 1;
}

void read_fread(FILE *fi)
{
	char tmp[SZ + 1];
	unsigned a, j = 0;
	long res;
	while ((a = fread(tmp, sizeof(tmp[0]), SZ, fi))) {
		tmp[a] = 0;
		res = whereis(tmp, '\n');
		if (res > 0) { // found newline
			while ((res = whereis(tmp + j, '\n')) > -1) {
				apnd_s(*it, tmp + j, res);
				j += res;
				if (++curnum >= txt_cpt) {
					txt_cpt *= 2;
					text.resize(txt_cpt);
				}
				++it;
			}
		}
		// if last character is not a newline
		apnd_s(*it, tmp + j, a - j);
		j = 0;
	}
}
