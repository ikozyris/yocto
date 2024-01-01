#include "io.cpp"
#include "sizes.c"

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
				fscanf(file, " %d", &memusg);
			else if (strcmp(buffer, "Pid:") == 0)
				fscanf(file, " %d", &pid);
		}
		fclose(file);
		sprintf(buffer, "RAM: %s pid: %d",
			  hrsize(memusg * 1000), pid);
		clear_header();
		print2header(buffer, 1);
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
	fputws(data(i, i.capacity), fo);
#else
	fputs(data(i, i.capacity), fo);
#endif
	fclose(fo);

	reset_header();
	print2header("Saved", 1);
	wmove(text_win, y, x);
}

void info()
{
	char tmp[42];
	struct stat stat_buf;
	if (filename && stat(filename, &stat_buf) == 0) {
		sprintf(tmp, "%ld lines", curnum + 1);
		print2header(hrsize(stat_buf.st_size), 3);
		print2header(tmp, 1);
	}
	sprintf(tmp, "y: %u x: %u lenght: %u", 
		ry, x, it->length);
	print2header(tmp, 2);
}

void enter()
{
	insert(*it, x, '\n');

	gap_buf *t = (gap_buf*)malloc(sizeof(gap_buf));
	apnd_s(*t, data(*it, x, (*it).length),
		(*it).length - x);

	for (unsigned i = (*it).gpe; i < (*it).length + (*it).gpe; ++i)
		eras(*it, i);
	text.insert(++it, *t);
	++curnum;
	// print text again (or find a better way)
	print_text();
	wmove(text_win, y + 1, 0);
}