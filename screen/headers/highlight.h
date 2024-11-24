#pragma once
#include "init.h"

extern bool eligible; // is syntax highlighting enabled
extern const char *types[];
extern const char *defs[];
extern const char oper[];
extern unsigned char types_len[];
extern unsigned char defs_len[];
extern wchar_t tmp[];
extern char str[];

#define DEFINC	COLOR_CYAN
#define COMMENT	COLOR_GREEN
#define TYPES	COLOR_RED
#define OPER	COLOR_YELLOW
#define DEFS    COLOR_BLUE
#define STR	COLOR_MAGENTA
// TODO: color for numbers?

#define nelems(x)  (sizeof(x) / sizeof((x)[0]))
#define is_separator(ch) ((ch > 31 && ch < 48) || (ch > 57 && ch < 65) || (ch > 90 && ch < 95) || ch > 122)

typedef struct res_s {
	unsigned char len;
	char type;
} res_t;


bool isc(const char *str);
res_t get_category(const char *line);
void apply(unsigned line); // highight line if eligible = true
void highlight(unsigned y); // helper function for apply()
