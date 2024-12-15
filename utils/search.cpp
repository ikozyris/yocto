#include "headers/search.h"

void find(const char *str)
{
	unsigned short tmp_len = strlen(str);
	if (tmp_len == 0)
		return;
	vector<unsigned> matches = search(*it, str, tmp_len);
	tmp_len -= mbcnt(str, tmp_len); // get displayed characters
	snprintf(lnbuf, lnbf_cpt, "%lu matches     ", matches.size());
	print2header(lnbuf, 1);
	curs_set(0);
	unsigned dx = 0, previ = 0, prevpr = 0;
	for (unsigned i = 0; i < matches.size(); ++i) { // highlight all
		calc_offset_act(matches[i], previ, *it);
		previ = matches[i];
		dx += flag;
		if (dx >= maxx - 1 + prevpr) {
			int key = wgetch(text_win); // quit
			if (key == 'q' || key == 27)
				goto clr_high;
			prevpr = previ - previ % (maxx - 1);
			mvprint_line(y, 0, *it, prevpr, 0);
		}
		wmove(text_win, y, dx % (maxx - 1));
		wchgat(text_win, tmp_len, A_STANDOUT, 0, 0);
	}
	wgetch(text_win); // give time to view results
clr_high:
	mvprint_line(y, 0, *it, 0, 0);
	curs_set(1);
}

unsigned *_badchar(const char *str, unsigned short len)
{
	unsigned *badchar = (unsigned*)malloc(256 * sizeof(unsigned));
	for (unsigned i = 0; i < 256; ++i)
		badchar[i] = len;
	for (unsigned i = 0; i < len; i++)
		badchar[(unsigned char)str[i]] = len - i - 1;

	return badchar;
}

unsigned *_goodsuffix(const char *str, unsigned short len)
{
	unsigned *gs = (unsigned*)malloc(len * sizeof(unsigned));
	int *pos = (int*)malloc(len * sizeof(int));
	fill(pos, pos + len, -1);

	for (unsigned i = 1; i < len; i++) {
		int j = pos[i - 1];
		while (j >= 0 && str[i] != str[j])
			j = pos[j];
		pos[i] = j + 1;
	}

	for (unsigned i = 0; i < len; i++)
		gs[i] = len - pos[i];

	// Apply the Galil rule
	for (unsigned i = len - 1; i > 0; i--) {
		if (str[i] != str[pos[i]])
			gs[i] = len - i;
		else
			gs[i] = gs[pos[i]];
	}
	gs[0] = len;
	free(pos);
	return gs;
}
vector<unsigned> bm_search(const gap_buf &buf, const char *str, unsigned short len) {
	vector<unsigned> matches;
	// Preprocess the str to create the bad character table
	unsigned *badchar = _badchar(str, len);
	unsigned *goodsuffix = _goodsuffix(str, len);

	for (unsigned i = 0; i < buf.len - len;) {
		unsigned j;

		// check from end of str
		for (j = len - 1; j < len && str[j] == at(buf, i + j); --j);
		
		if (j > len) { // unsigned overflow
			matches.push_back(i);
			i += len; // no overlaps
		} else
			i += max(badchar[(unsigned char)at(buf, i + j)], goodsuffix[j]);
	}
	free(goodsuffix);
	free(badchar);
	return matches;
}

// merge the sorted results of each thread (this is the bottleneck)
vector<unsigned> mergei(const vector<vector<unsigned>> &indices)
{
	vector<unsigned> matches;
	matches.reserve(indices[0].size());
	for (const auto& vec : indices)
		matches.insert(matches.end(), vec.begin(), vec.end());
	return matches;
}

// each thread searches with this
void searchch(const gap_buf &buf, char ch, unsigned st, unsigned end, vector<unsigned> &matches)
{
	for (unsigned i = st; i < end; ++i)
		if (at(buf, i) == ch)
			matches.push_back(i);
}

// wrapper for searchch() to launch with multi-threaded
vector<unsigned> mt_search(const gap_buf &buf, char ch)
{
	unsigned nthreads = thread::hardware_concurrency();
	if (nthreads == 0)
		nthreads = 2;
	unsigned chunk = buf.len / nthreads;
	vector<thread> threads(nthreads);
	vector<vector<unsigned>> indices(nthreads); // each thread's result

	for (unsigned i = 0; i < nthreads - 1; ++i) {
		unsigned st = i * chunk;
		unsigned end = (i + 1) * chunk;

		threads.emplace_back(searchch, ref(buf), ch, st, end, ref(indices[i]));
	}
	// last thread does the remaining (until buf.len)
	threads.emplace_back(searchch, ref(buf), ch, (nthreads - 1) * chunk, buf.len, ref(indices[nthreads - 1]));

	for (auto& thread : threads)
		if (thread.joinable())
			thread.join();

	vector<unsigned> matches = mergei(indices);
	return matches;
}

// search for str in buf, return <pos, color(pos;s)>
vector<unsigned> search(const gap_buf &buf, const char *str, unsigned short len)
{
	vector<unsigned> matches;
	if (len == 1) {
		//searchch(buf, str[0], 0, buf.len, matches);
		matches = mt_search(buf, str[0]);
	} else
		matches = bm_search(buf, str, len);

	return matches;
}

