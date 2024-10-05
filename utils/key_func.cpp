#include "io.cpp"

void stats()
{
	char *_tmp = (char*)malloc(256);
	unsigned sumlen = 0;
	for (auto &i : text)
		sumlen += i.len;
	//snprintf(_tmp, min(maxx, 256), "maxx %u len %u ofx %ld wrap %ld x: %u | y: %u     ",
	//	maxx, it->len, ofx, wrap, x, y);
	snprintf(_tmp, min(maxx, 256), "length %u y %u x %u sum len %u lines %lu ofx %ld  ", 
	it->len, ry, x, sumlen, curnum, ofx);
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
		size_t bytes_saved = 0;
		for (auto &i : text)
			bytes_saved += shrink(i);
		char *bytes_saved_str = (char*)malloc(24);
		hrsize(bytes_saved, bytes_saved_str, 24);

		size_t curr = memusg(); // RAM usage after
		char *prev_curr_str = (char*)malloc(24);
		hrsize((prev - curr) * 1000, prev_curr_str, 24);

		char buffer[64] = "";
		snprintf(buffer, 64, "saved: %s | %s", bytes_saved_str, prev_curr_str);
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

void eol()
{
	ofx = calc_offset_act(it->len, 0, *it);
	if (it->len <= (long)maxx + ofx) // line fits in screen
		wmove(text_win, y, it->len - ofx);
	else { // wrap line
		wmove(text_win, y, 0);
		wclrtoeol(text_win);
		unsigned vis = (it->len - ofx) % (maxx - 1);
		unsigned bytes = calc_offset_dis(it->len - ofx - vis, 0, *it) + it->len - ofx - vis;
		print_line(*it, bytes, it->len);
		ofx = wrap = (long)it->len - (long)vis;
	}
}

void sol()
{
	wmove(text_win, y, 0);
	if (wrap) { // line has been wrapped
		wclrtoeol(text_win);
		print_line(*it);
#ifdef HIGHLIGHT
		apply(y);
#endif
		wmove(text_win, y, 0);
	}
	wrap = ofx = 0;
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
	apply(y);
#endif
	// TODO: calc offset and use current x
	wmove(text_win, y, 0);
}

void scrollup()
{
	wscrl(text_win, -1);
	wscrl(ln_win, -1);
	mvwprintw(ln_win, 0, 0, "%3u", ry);
	--ofy;
	wmove(text_win, 0, 0);
	--it;
	print_line(*it);
#ifdef HIGHLIGHT
	apply(y);
#endif
	wmove(text_win, 0, x);
	wrefresh(ln_win);
	ofx = 0;
}

// TODO: is all this complexity needed?
void left()
{
	if (x == 0 && ofx > 0) { // line has been wrapped
		wmove(text_win, y, 0);
		wclrtoeol(text_win);
		if (wrap == 0) // line has been partially unwrapped
			wrap = maxx - 1;
		ofx -= wrap;
		print_line(*it, ofx);
		wrap = 0;
#ifdef HIGHLIGHT
		apply(y);
#endif
		wmove(text_win, y, maxx - 1);
	} else if (x > 0) {
		// TODO: use prevword() to get actual offset
		if (it->buffer[it->gps - 1] == '\t') {
			wmove(text_win, y, x - 8);
			ofx += 7;
			return;
		}
		wmove(text_win, y, x - 1);
		if (it->buffer[it->gps - 1] < 0)
			--ofx;
	} else if (y > 0) { // x = 0
		if (wrap > 0) // revert wrap
			print_line(*it);
		--it;
		--y;
		eol();
	}
}

void right()
{
	if (it->buffer[it->gpe + 1] == '\t') {
		if (x > maxx - 7)
			goto wrap_line;
		ofx -= 8 - x % 8 - 1;
		wmove(text_win, y, x + 8 - x % 8);
	} else if (x == maxx - 1 && rx < it->len - 1) {
wrap_line:
		wmove(text_win, y, 0);
		wclrtoeol(text_win);
		ofx += (wrap = maxx - 1);
		print_line(*it, ofx);
		wmove(text_win, y, 0);
	} else {
		if (ry < curnum) {
			if (rx < it->len - 1) {
right_key:
				wmove(text_win, y, x + 1);
				if (it->buffer[it->gpe + 1] < 0)
					++ofx;
			} else {
				wmove(text_win, y, 0);
				if (wrap > 0) // revert wrap
					print_line(*it);
				wmove(text_win, y + 1, 0);
				++it;
				wrap = ofx = 0;
			}
		} else if (ry == curnum && rx < it->len)
			goto right_key;
	}	
}
