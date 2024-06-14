#include "io.cpp"

void stats()
{
	char *_tmp = (char*)malloc(256);
	struct stat stat_buf;
	if (filename && stat(filename, &stat_buf) == 0) {
		sprintf(_tmp, "%ld lines", curnum + 1);
		print2header(hrsize(stat_buf.st_size), 3);
		print2header(_tmp, 1);
	}
	unsigned sumlen = 0;
	for (auto &i : text)
		sumlen += i.len;
	snprintf(_tmp, maxx-2, "gap start %u g end %u buffer size %u length %u y %u x %u sum len %u", 
		it->gps, it->gpe, it->cpt, it->len, ry, x, sumlen);
	clear_header();
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
                txt_cpt = curnum + 1;
                text.resize(txt_cpt);
		for (auto &i : text)
			shrink(i);
	} else if (strcmp(tmp, "usage") == 0) {
		size_t memusg = 0, tmp, pid;
		// stores each word in status file
		char buffer[1024] = "";

		// Linux-only
		FILE *file = fopen("/proc/self/status", "r");
		if (!file)
			return;

		while (fscanf(file, " %1023s", buffer) == 1)
			if (strcmp(buffer, "Pid:") == 0) {
				fscanf(file, " %lu", &pid);
				break;
			}
		fclose(file);
		file = fopen("/proc/self/smaps", "r");
		while (fscanf(file, " %1023s", buffer) == 1)
			if (strcmp(buffer, "Pss:") == 0) {
				fscanf(file, " %lu", &tmp);
				memusg += tmp;
			}
		fclose(file);
		sprintf(buffer, "RAM: %s PID: %lu", hrsize(memusg * 1000), pid);
		clear_header();
		print2header(buffer, 1);
	} else if (strcmp(tmp, "stats") == 0)
		stats();
	else if (strcmp(tmp, "run") == 0) {
		char *_tmp = input_header("Enter command");
		system(_tmp);
		free(_tmp);
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
		snprintf(tmp, maxx, "%u secs %u millisecs (%u iterations)\n",
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
		bzero(temp, 256 * sizeof(wchar_t));
		winwstr(text_win, temp);
		unsigned len = wcstombs(it->buffer, temp, it->cpt);
		it->len = len;
		it->gps = len + 1;
		it->gpe = it->cpt;
	} else
		print2header("command not found", 3);
	free(tmp);
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

void enter()
{
	insert_c(*it, rx, '\n');

	gap_buf *t = (gap_buf*)malloc(sizeof(gap_buf));
	init(*t);
	if ((it->cpt != it->gpe)) { // newline is not at the end
		char *tmp = data2(*it, rx + 1, it->len + 1);
		apnd_s(*t, tmp, it->len - rx - 1);
		free(tmp);
		it->gps = rx + 1;
		it->len = rx + 1;
		it->gpe = it->cpt;
	}

	// somewhere below iterator is invalidated
	++it;
	++curnum;
        ++ofy;
	text.insert(it, *t);
	free(t);
	// print text again (or find a better way)
	print_text();
	wmove(text_win, y + 1, 0);
	// TODO: this shouldn't be necessary
	it = text.begin();
	std::advance(it, ry + 1);
}
