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
	const char *tmp = input_header("Enter command: ");
	if (strcmp(tmp, "resethead") == 0)
		reset_header();
	else if (strcmp(tmp, "shrink") == 0)
		for (auto i : text)
			shrink(i);
	else if (strcmp(tmp, "usg") == 0) {
		int memusg, pid;
		// stores each word in status file
		char buffer[1024] = "";

		// Linux-only
		FILE *file = fopen("/proc/self/status", "r");
		if (!file)
			return;

		// read the entire file
		while (fscanf(file, " %1023s", buffer) == 1) {
			if (strcmp(buffer, "VmRSS:") == 0)
				fscanf(file, " %d", &memusg); // in kB
			else if (strcmp(buffer, "Pid:") == 0)
				fscanf(file, " %d", &pid);
		}
		fclose(file);
		sprintf(buffer, "RAM: %s PID: %d", hrsize(memusg * 1000), pid);
		clear_header();
		print2header(buffer, 1);
	} else if (strcmp(tmp, "stats") == 0)
		stats();
	else if (strcmp(tmp, "run") == 0) {
		const char *_tmp = input_header("Enter command");
		system(_tmp);
	} else if (strcmp(tmp, "help")  == 0)
		print2header("resetheader, shrink, usg, stats, run, setgap", 1);
	else if (strcmp(tmp, "setgap") == 0) {
		const char *in = input_header("Gap start: ");
		sscanf(in, "%u", &(it->gps));
	} else if (strcmp(tmp, "fixgap") == 0) {
		mv_curs(*it, it->len);
		mv_curs(*it, x);
	} else
		print2header("command not found", 3);
}

void save()
{
	if (!filename)
		filename = (char*)malloc(128);
	if (strlen(filename) <= 0)
		filename = (char*)input_header("Enter filename: ");
	FILE *fo = fopen(filename, "w");
	std::list<gap_buf>::iterator iter;
	for (iter = text.begin(); iter != text.end(); ++iter)
		fputs(data((*iter), 0, (*iter).cpt), fo);
	fclose(fo);

	reset_header();
	print2header("Saved", 1);
	wmove(text_win, y, x);
}

void enter()
{
	insert_c(*it, x, '\n');

	gap_buf *t = (gap_buf*)malloc(sizeof(gap_buf));
	init(*t);
	//char tmp[it->len];
	//bzero(tmp, it->len);
	//for (int i = it->gpe+1; i < it->len + gaplen(*it); ++i)
	//	tmp[i-(it->gpe+1)] = it->buffer[i];
	//apnd_s(*t, tmp);
	//apnd_s(*t, data(*it, it->gpe+1, it->len + gaplen(*t)));	
	if (it->gpe != it->cpt) { // newline is at the end
		apnd_s(*t, data2(*it, x+1, it->len+1));
		it->gps = x + 1;
		it->len = x + 1;
		it->gpe = it->cpt;
	}

	// somewhere below iterator is invalidated
	++it;
	++curnum;
	text.insert(it, *t);
	free(t);
	// print text again (or find a better way)
	print_text();
	wmove(text_win, y + 1, 0);
	// TODO: this shouldn't be necessary
	it = text.begin();
	std::advance(it, y+1);
}
