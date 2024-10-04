#include "init.c"

bool eligible;

bool isc(const char *str)
{
	long res = whereis(str, '.');
	if (res == -1)
		return false;
	str += res;
	if (!strcmp(str, "c") || !strcmp(str, "cpp") || !strcmp(str, "cc")
		|| !strcmp(str, "h") || !strcmp(str, "hpp"))
		return true;
	return false;
}

const char *types[] = {"int", "char", "float", "double", "unsigned", "void", "const", 
	"size_t", "bool", "signed", "long", "enum", "static", "short", "extern"};
const char *defs[]  = {"if", "else", "while", "for", "do", "return", "sizeof", "switch",
	"goto", "case", "break", "struct", "default", "continue", "true", "false"};
const char *oper[]  = {"=", "+=", "+", "-", "-=", "*", "*=", "/", "/=", "%%", "&", "++",
	"--", "==", "<", ">", "[", "]"};

#define DEFINC	COLOR_CYAN
#define COMMENT	COLOR_GREEN
#define TYPES	COLOR_RED
#define OPER	COLOR_YELLOW
#define DEFS    COLOR_BLUE
#define STR	COLOR_MAGENTA
// TODO: color for numbers?

#define nelems(x)  (sizeof(x) / sizeof((x)[0]))
#define isvalid(ch) (ch < 65 || ch > 122)

struct res_t {
	unsigned short len;
	char type;
};

struct res_t get_category2(const char *line)
{
	struct res_t res;
	res.len = 0;
	res.type = 0;

	unsigned j = 0;
	bool found = true;
	for (unsigned i = 0; i < nelems(types); ++i) {
		found = true;
		for (j = 0; types[i][j] != 0; ++j)
			if (types[i][j] != line[j])
				found = false;
		if (found == true) {
			res.len = strlen(types[i]);
			res.type = 't';
		}
	}
	for (unsigned i = 0; i < nelems(defs); ++i) {
		found = true;
		for (j = 0; defs[i][j] != 0; ++j)
			if (defs[i][j] != line[j])
				found = false;
		if (found == true) {
			res.len = strlen(defs[i]);
			res.type = 'd';
		}
	}
	for (unsigned i = 0; i < nelems(oper); ++i) {
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

wchar_t tmp[256];
char str[256];

void apply(unsigned line)
{
	if (!eligible)
		return;
	winwstr(text_win, tmp);
	unsigned len2 = wcstombs(str, tmp, 256);
	unsigned previ = 0;
	len2 = min(len2, maxx);

	for (unsigned i = 0; i < len2; ++i) {
		if (str[i] == '#') { // define / include
			wmove(text_win, line, i);
			wchgat(text_win, -1, 0, DEFINC, 0);
			return;
		} else if (str[i] == '/' && str[i + 1] == '/') { // comments
			wmove(text_win, line, i);
			wchgat(text_win, -1, 0, COMMENT, 0);
			return;
		} else if (str[i] == '/' && str[i + 1] == '*') {
			previ = i;
			i += 2;
			wmove(text_win, line, previ);
			while (str[i] != '*' && str[i + 1] != '/' && i < len2)
				++i;
			wchgat(text_win, i++ - previ + 2, 0, COMMENT, 0);
		} else if (str[i] == '\'') { // string / char
			previ = i++;
			wmove(text_win, line, previ);
			while (str[i] != '\'' && i < len2)
				++i;
			wchgat(text_win, i - previ + 1, 0, STR, 0);
		} else if (str[i] == '\"') {
			previ = i++;
			wmove(text_win, line, previ);
			while (str[i] != '\"' && i < len2)
				++i;
			wchgat(text_win, i - previ + 1, 0, STR, 0);
		} else { // type (int, char) / keyword (if, return) / operator (==, +)
			wmove(text_win, line, i);
			struct res_t res = get_category2(str + i);
			if (res.type == 't' && isvalid(str[i + res.len])
				&& (i == 0 ? 1 : isvalid(str[i - 1])))
				wchgat(text_win, res.len, 0, TYPES, 0);
			else if (res.type == 'd' && isvalid(str[i + res.len])
				&& (i == 0 ? 1 : isvalid(str[i - 1])))
				wchgat(text_win, res.len, 0, DEFS, 0);
			else if (res.type == 'o')
				wchgat(text_win, res.len, 0, OPER, 0);
		}
	}
}
