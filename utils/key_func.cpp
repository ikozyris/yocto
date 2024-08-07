#include "io.cpp"

void stats()
{
	char *_tmp = (char*)malloc(256);
	unsigned sumlen = 0;
	for (auto &i : text)
		sumlen += i.len;
	snprintf(_tmp, maxx-2, "length %u y %u x %u sum len %u lines %lu ofx %ld  ", 
		it->len, ry, x, sumlen, curnum, ofx);
	print2header(_tmp, 1);
	free(_tmp);
	_tmp = 0;
	wmove(text_win, y, x);
}

void command()
{
	char *tmp = input_header("Enter command: ");
	if (strcmp(tmp, "resetheader") == 0)
		reset_header();
	else if (strcmp(tmp, "shrink") == 0) {
		size_t prev = memusg();

		// shrink line buffer
		lnbf_cpt = 16;
		lnbuf = (char*)realloc(lnbuf, lnbf_cpt);
                // shrink linked list
		txt_cpt = curnum + 1;
                text.resize(txt_cpt);
		//shrink each line
		for (auto &i : text)
			shrink(i);

		size_t curr = memusg();
		char buffer[1024] = "";
		sprintf(buffer, "saved: %s", hrsize((prev-curr) * 1000));
		clear_header();
		print2header(buffer, 1);
	} else if (strcmp(tmp, "usage") == 0) {
		size_t pid;
		// stores each word in status file
		char buffer[1024] = "";

		// Linux-only
		FILE *file = fopen("/proc/self/status", "r");
		while (fscanf(file, " %1023s", buffer) == 1)
			if (strcmp(buffer, "Pid:") == 0) {
				fscanf(file, " %lu", &pid);
				break;
			}
		fclose(file);
		sprintf(buffer, "RAM: %s PID: %lu", hrsize(memusg() * 1000), pid);
		clear_header();
		print2header(buffer, 1);
	} else if (strcmp(tmp, "stats") == 0)
		stats();
	else if (strcmp(tmp, "run") == 0) {
		char *_tmp = input_header("Enter command: ");
		clear();
		refresh();
		int res = system(_tmp);
		if (res != 0)
			print2header(itoa(res), 1);
		free(_tmp);
		getch();
		reset_header();
		print_text(0);
		print_lines();
		wnoutrefresh(ln_win);
		wnoutrefresh(header_win);
		doupdate();
		wmove(text_win, y, x);
	} else if (strcmp(tmp, "help")  == 0)
		print2header("resetheader, shrink, usg, stats, run, setgap", 1);
	else if (strcmp(tmp, "setgap") == 0)
		++it->gps;
	else if (strcmp(tmp, "fixgap") == 0) {
		unsigned msec = 0, trigger = 1; /* 1ms */
		unsigned iterations = 0;
		clock_t before = clock();
		do {
			mv_curs(*it, it->len);
			mv_curs(*it, x);
			clock_t difference = clock() - before;
			msec = difference * 1000 / CLOCKS_PER_SEC;
			iterations++;
		} while (msec < trigger);
		char tmp[128];
		snprintf(tmp, maxx, "%u,%03u ms (%u iterations)",
  			msec / 1000, msec % 1000, iterations);
		print2header(tmp, 1);
		wmove(text_win, y, x);
	} else if (strcmp(tmp, "scroll") == 0) {
		char *in = input_header("Scroll to line: ");
		unsigned a;
		sscanf(in, "%u", &a);
		free(in);
		if (a <= curnum) {
			ofy = a - 1;
			print_lines();
			wrefresh(ln_win);
			print_text(0);
			std::advance(it, ofy - ry);
		}
	} else if (strcmp(tmp, "fixstr") == 0) {
		wchar_t temp[256];
		winwstr(text_win, temp);
		wcstombs(it->buffer, temp, it->cpt);
		it->len = sizeofline(y);
		it->gps = it->len + 1;
		it->gpe = it->cpt;
	} else
		print2header("command not found", 3);
	free(tmp);
}

void enter()
{
	insert_c(*it, rx, '\n');

	gap_buf *t = (gap_buf*)malloc(sizeof(gap_buf));
	init(*t);
	if (it->gpe < it->cpt - 1) { // newline is not at the end
		data2(*it, rx + 1, it->len + 1);
		apnd_s(*t, lnbuf, it->len - rx - 1);
		it->gps = rx + 1;
		it->len = rx + 1;
		it->gpe = it->cpt - 1;
	}

	// somewhere below iterator is invalidated
	++it;
	++curnum;
	text.insert(it, *t);
	--it;
	free(t);
	print_text(y);
	if (y < maxy - 1)
		wmove(text_win, y + 1, 0);
	else {
		wscrl(text_win, 1);
		++ofy;
		wmove(text_win, maxy - 1, 0);
		print_line(*it);
		wmove(text_win, maxy - 1, x);
	}
	ofx = 0;
}

// TODO: handle better lines with uncalculated offsets
void eol()
{
	if (it->len < (long)maxx + ofx) {
		x = sizeofline(y);
		ofx = (long)it->len - (long)x;
	} else { // wrap line
		wmove(text_win, y, 0);
		wclrtoeol(text_win);
		// TODO: use actual printline()
		data2(*it, it->len - maxx - ofx, it->len);
		waddnstr(text_win, lnbuf, maxx + ofx - 1);
		ofx = (long)it->len - (long)maxx - ofx;
	}
}

void scrolldown()
{
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

}

void scrollup()
{
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
}
