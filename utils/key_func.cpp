#include "io.cpp"

void stats()
{
	char *_tmp = (char*)malloc(256);
	unsigned sumlen = 0;
	for (auto &i : text)
		sumlen += i.len;
#ifndef RELEASE
	unsigned wrapd = 0, wrapb = 0;
	if (!wrap.empty()) {
		wrapb = wrap.back().byte;
		wrapd = wrap.back().dchar;
	}
	snprintf(_tmp, min(maxx, 256), "maxx %u len %u cpt %u wrap[d%u,b%u] x: %u ofx: %ld ry: %u     ",
		maxx, it->len, it->cpt, wrapd, wrapb, x, ofx, ry);
#else	
	snprintf(_tmp, min(maxx, 256), "length %u cpt %u y %u x %u sum len %u lines %lu wrap %lu  ", 
		it->len, it->cpt, ry, x, sumlen, curnum, wrap.size());
#endif
	print2header(_tmp, 1);
	free(_tmp);
	wmove(text_win, y, x);
}

void command()
{
	char *tmp = input_header("Enter command: ");
	if (strcmp(tmp, "resetheader") == 0)
		reset_header();
	else if (strcmp(tmp, "shrink") == 0) {
		size_t prev = memusg(); // RAM usage before

		// shrink line buffer
		lnbf_cpt = 16;
		lnbuf = (char*)realloc(lnbuf, lnbf_cpt);
                // shrink linked list
		txt_cpt = curnum + 1;
                text.resize(txt_cpt);
		//shrink each line
		size_t bytes_freed = 0;
		for (auto &i : text)
			bytes_freed += shrink(i);
		char *bytes_saved_str = (char*)malloc(24);
		hrsize(bytes_freed, bytes_saved_str, 24);

		size_t curr = memusg(); // RAM usage after
		char *prev_curr_str = (char*)malloc(24);
		hrsize((prev - curr) * 1000, prev_curr_str, 24);

		char buffer[64] = "";
		snprintf(buffer, 64, "freed: %s | kernel reclaimed %s", bytes_saved_str, prev_curr_str);
		free(bytes_saved_str);
		free(prev_curr_str);
		clear_header();
		print2header(buffer, 1);
	} else if (strcmp(tmp, "usage") == 0) {
		size_t pid = 0;
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
		char ram_usg[24];
		hrsize(memusg() * 1000, ram_usg, 24);
		sprintf(buffer, "RAM: %s PID: %lu", ram_usg, pid);
		clear_header();
		print2header(buffer, 1);
	} else if (strcmp(tmp, "stats") == 0)
		stats();
	else if (strcmp(tmp, "run") == 0) {
		char *_tmp = input_header("Enter command: ");
		clear();
		refresh();
		int res = system(_tmp);
		free(_tmp);
		printw("\nreturned: %d", res);

		getch();
		reset_header();
		print_text(0);
		print_lines();
		wnoutrefresh(ln_win);
		wnoutrefresh(header_win);
		doupdate();
		wmove(text_win, y, x);
	} else if (strcmp(tmp, "help")  == 0)
		print2header("resetheader, shrink, usage, stats, run, scroll", 1);
	else if (strcmp(tmp, "scroll") == 0) {
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
	} else
		print2header("command not found", 3);
	free(tmp);
}

void enter()
{
	insert_c(*it, rx, '\n');
	gap_buf *t = (gap_buf*)malloc(sizeof(gap_buf));
	init(*t);
	if (it->gpe < it->cpt - 2) { // newline is not at the end
		data(*it, rx + 1, it->len + 1);
		apnd_s(*t, lnbuf, it->len - rx - 1);
		it->len = it->gps = rx + 1;
		it->gpe = it->cpt - 1;
	}
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
		mvprint_line(maxy - 1, 0, *it, 0, 0);
		wmove(text_win, maxy - 1, x);
	}
	ofx = 0;
}

void eol()
{
	ofx = calc_offset_act(it->len, 0, *it);
	if (it->len - ofx <= maxx) // line fits in screen
		wmove(text_win, y, it->len - ofx - 1);
	else { // cut line
		wrap.clear();
		unsigned bytes = 0, nbytes = 0;
		while (bytes < it->len) {
			nbytes = dchar2bytes(maxx - 1, bytes, *it);
			if (nbytes >= it->len - 1)
				break;
			wrap.push_back({flag , nbytes}); // flag was changed by dchar2bytes
			ofx += flag;
			bytes = nbytes;
		}
		wrap.back().byte--; // newline
		clearline;
		print_line(*it, bytes, it->len);
		// TODO: is this still needed?
		x = getcurx(text_win);
		if (x + ofx > it->len - 1) 
			ofx -= x + ofx - it->len + 1;
	}
}

void sol()
{
	if (!wrap.empty()) { // line has been wrapped
		clearline;
		print_line(*it);
		highlight;
	}
	wrap.clear();
	wmove(text_win, y, ofx = 0);
}

void scrolldown()
{
	ofy++;
	wscrl(text_win, 1);
	wscrl(ln_win, 1);
	mvwprintw(ln_win, maxy - 1, 0, "%3u", ry + 2);
	wrefresh(ln_win);
	mvprint_line(y, 0, *it, 0, 0);
	highlight;
	wmove(text_win, y, 0);
	wrap.clear();
	ofx = 0;
}

void scrollup()
{
	wscrl(text_win, -1);
	wscrl(ln_win, -1);
	mvwprintw(ln_win, 0, 0, "%3u", ry);
	--ofy;
	--it;
	mvprint_line(0, 0, *it, 0, 0);
	highlight;
	wmove(text_win, 0, x);
	wrefresh(ln_win);
	wrap.clear();
	ofx = 0;
}

void left()
{
	if (x == 0 && !wrap.empty()) { // line has been wrapped
		clearline;
		ofx -= wrap.back().dchar;
		wrap.pop_back();
		print_line(*it, wrap.empty() ? 0 : wrap.back().byte);
		highlight;
		wmove(text_win, y, flag + 1);
	} else if (x > 0) {
		wmove(text_win, y, x - 1);
		// handle special characters causing offsets
		if (it->buffer[it->gps - 1] == '\t')
			prevdchar();
		else if (it->buffer[it->gps - 1] < 0)
			--ofx;
	} else if (y > 0) { // x = 0
		--it;
		--y;
		eol();
	}
}

void right()
{
	if (rx >= it->len - 1 && ry < curnum) { // go to next line
		if (!wrap.empty()) // revert wrap
			mvprint_line(y, 0, *it, 0, 0);
		wmove(text_win, y + 1, 0);
		++it;
		wrap.clear();
		ofx = 0;
	} else if (x == maxx - 1) { // right to cut part of line
wrap_line:
		clearline;
		ofx += x;
		wrap.push_back({x, (wrap.empty() ? 0 : wrap.back().byte) + print_line(*it, ofx)});
		wmove(text_win, y, 0);
	} else { // go right
		wmove(text_win, y, x + 1);
		if (it->buffer[it->gpe + 1] == '\t') {
			if (x >= maxx - 7)
				goto wrap_line;
			ofx -= 8 - x % 8 - 1;
			wmove(text_win, y, x + 8 - x % 8);
		} else if (it->buffer[it->gpe + 1] < 0)
			++ofx;
	}		
}
