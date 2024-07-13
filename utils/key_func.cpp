#include "io.cpp"

long memusg()
{
	size_t memusg = 0, tmp;
	char buffer[1024];
	FILE *file = fopen("/proc/self/smaps", "r");
	while (fscanf(file, " %1023s", buffer) == 1)
		if (strcmp(buffer, "Pss:") == 0) {
			fscanf(file, " %lu", &tmp);
			memusg += tmp;
		}
	fclose(file);
	return memusg;
}

void stats()
{
	char *_tmp = (char*)malloc(256);
	unsigned sumlen = 0;
	for (auto &i : text)
		sumlen += i.len;
	snprintf(_tmp, maxx-2, "length %u y %u x %u sum len %u lines %lu ", 
		it->len, ry, x, sumlen, curnum);
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
                txt_cpt = curnum + 1;
                text.resize(txt_cpt);
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
		print_text();
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
			print_text();
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
	if (it->cpt != it->gpe) { // newline is not at the end
		data2(*it, rx + 1, it->len + 1);
		apnd_s(*t, lnbuf, it->len - rx - 1);
		it->gps = rx + 1;
		it->len = rx + 1;
		it->gpe = it->cpt;
	}

	// somewhere below iterator is invalidated
	++it;
	++curnum;
	text.insert(it, *t);
	free(t);
	if (ry >= maxy - 1) {
		++ofy;
		print_lines();
		wnoutrefresh(ln_win);
	}
	// print text again (or find a better way)
	print_text();
	if (y < maxy-1)
		wmove(text_win, y + 1, 0);
	ofx = 0;
	// TODO: this shouldn't be necessary
	it = text.begin();
	std::advance(it, ry + 1);
}
