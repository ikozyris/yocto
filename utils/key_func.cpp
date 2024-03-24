#include "io.cpp"

void info()
{
	char tmp[42];
	struct stat stat_buf;
	if (filename && stat(filename, &stat_buf) == 0) {
		sprintf(tmp, "%ld lines", curnum + 1);
		print2header(hrsize(stat_buf.st_size), 3);
		print2header(tmp, 1);
	}
	unsigned sumlen = 0;
	for (auto i : text)
		sumlen += i.len;
	sprintf(tmp, "y: %u x: %u len: %u sum len: %u", 
		ry, x, it->len, sumlen);
	print2header(tmp, 2);
}

void command()
{
	char *tmp = input_header("Enter command: ");
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
		sprintf(buffer, "RAM: %s PID: %d",
			  hrsize(memusg * 1000), pid);
		clear_header();
		print2header(buffer, 1);
	} else if (strcmp(tmp, "stats") == 0) {
		char tmp[42];
		sprintf(tmp, "s %u|  e %u| sz %u | len %u", 
			it->gps, it->gpe, it->cpt, it->len);
		clear_header();
		print2header(tmp, 1);
		wmove(text_win, y, x);
	} else if (strcmp(tmp, "run") == 0) {
		char *tmp = input_header("Enter command");
		system(tmp);
	} else if (strcmp(tmp, "info")  == 0) {
		info();
	} else if (strcmp(tmp, "help")  == 0) {
		print2header("resetheader, shrink, usg, stats, run, info, setgap", 1);
	} else if (strcmp(tmp, "setgap") == 0) {
		char *in = input_header("Gap start: ");
		sscanf(in, "%u", &(it->gps));
	} else {
		print2header("command not found", 3);
	}
}

void save()
{
	if (!filename)
		filename = (char*)malloc(128);
	if (strlen(filename) <= 0)
		filename = input_header("Enter filename: ");
	FILE *fo = fopen(filename, "w");
	for (auto i : text)
#if defined(UNICODE)
	fputws(data(i, 0, i.cpt), fo);
#else
	fputs(data(i, 0, i.cpt), fo);
#endif
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
	apnd_s(*t, data2(*it, x+1, it->len+1));
		//it->len - x);

	if (it->gpe != it->cpt) { // is newline not inserted at end
	 	for (unsigned i = it->gpe + 1; i < it->len + gaplen(*it); ++i)
	 		eras(*it, i);
		it->gps = x + 1;
		it->len = x + 1;
		it->gpe = it->cpt;
	}

	// somewhere below iterator is invalidated
	++it;
	++curnum;
	text.insert(it, *t);
	// print text again (or find a better way)
	print_text();
	wmove(text_win, y + 1, 0);
	// TODO: this shouldn't be necessary
	it = text.begin();
	std::advance(it, curnum);
}
