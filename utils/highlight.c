#include "init.c"

const char *types[] = {"int", "char", "float", "double", "unsigned", "void", "const", 
	"size_t", "signed", "long", "enum", "static", "short", "extern", 0};
const char *defs[]  = {"if", "else", "while", "for", "do", "return", "sizeof", "switch",
	"goto", "case", "break", "struct", "default", "continue", 0};
const char *oper[]  = {"=", "+=", "+", "-", "-=", "*", "*=", "/", "/=", "%%", "&", "++",
	"--", "==", "<", ">", "\'", 0};

#define RED     "\x1b[1;31m"
#define GREEN   "\x1b[1;32m"
#define YELLOW  "\x1b[1;33m"
#define BLUE    "\x1b[1;34m"
#define MAGENTA "\x1b[1;35m"
#define CYAN    "\x1b[1;36m"
#define RESET   "\x1b[0m"

struct res_t {
	short len;
	char type;
};

struct res_t get_category2(const char *line)
{
	struct res_t res;
	res.len = 0;
	res.type = 0;

	unsigned j = 0;
	bool found = true;
	for (unsigned i = 0; i < 14; ++i) {
		found = true;
		for (j = 0; types[i][j] != 0; ++j)
			if (types[i][j] != line[j])
				found = false;
		if (found == true) {
			res.len = strlen(types[i]);
			res.type = 't';
		}
	}
	for (unsigned i = 0; i < 14; ++i) {
		found = true;
		for (j = 0; defs[i][j] != 0; ++j)
			if (defs[i][j] != line[j])
				found = false;
		if (found == true) {
			res.len = strlen(defs[i]);
			res.type = 'd';
		}
	}
	for (unsigned i = 0; i < 17; ++i) {
		found = true;
		for (j = 0; oper[i][j] != 0; ++j)
			if (oper[i][j] != line[j])
				found = false;
		if (found == true) {
			res.len = strlen(oper[i]);
			res.type = 'o';
		}
	}
	return res;
}

void apply(unsigned line)
{
	wchar_t tmp[256];
	winwstr(text_win, tmp);
	char str[256];
	unsigned len2 = wcstombs(str, tmp, 256);
	unsigned previ = 0;

	for (unsigned i = 0; i < len2; ++i) {
		if (str[i] == '#') { // define / include
			wmove(text_win, line, i);
			wchgat(text_win, -1, 0, COLOR_CYAN, 0);
			return;
		} // comments
		else if (str[i] == '/' && str[i + 1] == '/') {
			wmove(text_win, line, i);
			wchgat(text_win, -1, 0, COLOR_GREEN, 0);
			return;
		} else if (str[i] == '/' && str[i + 1] == '*') {
			previ = i;
			i += 2;
			wmove(text_win, line, previ);
			while (str[i] != '*' && str[i + 1] != '/')
				++i;
			wchgat(text_win, i-previ+2, 0, COLOR_GREEN, 0);
		} // string / char
		else if (str[i] == '\'') {
			previ = i++;
			wmove(text_win, line, previ);
			while (str[i] != '\'')
				++i;
			wchgat(text_win, i-previ+1, 0, COLOR_MAGENTA, 0);
		} else if (str[i] == '\"') {
			previ = i++;
			wmove(text_win, line, previ);
			while (str[i] != '\"')
				++i;
			wchgat(text_win, i-previ+1, 0, COLOR_MAGENTA, 0);
		} // type (int, char) / keyword (if, return) / operator (==, +)
		else {
			wmove(text_win, line, i);
			struct res_t res = get_category2(str + i);
			if (res.type == 't')
				wchgat(text_win, res.len, 0, COLOR_RED, 0);
			else if (res.type == 'd')
				wchgat(text_win, res.len, 0, COLOR_BLUE, 0);
			else if (res.type == 'o')
				wchgat(text_win, res.len, 0, COLOR_YELLOW, 0);
		}
	}
}
