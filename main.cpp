#include <ncurses.h>
#include <cstdlib>
#include <cstring>
//#include <locale.h>
#include <list>
#include <vector>
#include <array>
#include <unistd.h>
#include "print_utils.cpp"
#include <locale>

#define ctrl(x)         ((x) & 0x1f)
#define MAX_LINES       5000
#undef KEY_ENTER
#define KEY_ENTER       10 // overwrite default
#define SIZE            20

using namespace std;

vector<list<wchar_t>> text(MAX_LINES);
size_t len[MAX_LINES]; //lenght of each line

int y = 0, x = 0;
// offset in y axis of text and screen 
signed int ofy = 0;
int maxy = 0, maxx = 0; // to store the maximum rows and columns
int i;
wchar_t ch;
list<wchar_t>::iterator it; // iterator
char filename[FILENAME_MAX];

// indx: tmp for lenght of line
// curnum: total lines
int indx = 0, curnum = 0, mx = -1;

int main(int argc, char *argv[])
{
        setlocale(LC_ALL, "");
        setlocale(LC_NUMERIC,"C");
        bzero(len, MAX_LINES);
        bzero(filename, FILENAME_MAX);

        // initialize curses
        initscr();
        cbreak();
        raw();
        noecho();
        refresh();

        getmaxyx(stdscr, maxy, maxx);

        // initialize header window
        WINDOW *header_win;
        header_win = newwin(1, maxx, 0, 0);
        wattrset(header_win, A_STANDOUT);
        print_header(header_win, maxx);
        wrefresh(header_win);

        //initialize line numbering window
        WINDOW *ln_win;
        ln_win = newwin(maxy, 3, 1, 0);
        wmove(ln_win, 0, 0);
        wattrset(ln_win, A_DIM);
        scrollok(ln_win, TRUE);
        for (i = 1;  i < maxy; ++i) 
                mvwprintw(ln_win, i-1, 0, "%2d", i);
        wrefresh(ln_win);

        //inititalize text_win window
        WINDOW *text_win;
        text_win = newwin(maxy - 1, maxx, 1, 3);
        scrollok(text_win, TRUE);
        keypad(text_win, TRUE);
        wmove(text_win, 0, 0);
        wrefresh(text_win);

        getmaxyx(text_win, maxy, maxx);

        wchar_t s[2];
        s[1] = 0;

        FILE *fo;
        if (argc > 1) {
                strcpy(filename, argv[1]);
                FILE *fp;
                fp = fopen(filename, "r");
                if (fp == NULL) {
                        perror("fopen");
                        return -2; // ENOENT
                }
                while ((ch = fgetwc(fp)) != EOF) {
                        text[curnum].push_back(ch);
                        s[0] = ch;
                        if (mx == -1 && indx < maxx)
                                wprintw(text_win, "%ls", s);
                                //waddch(text_win, ch); 
                        if (ch == '\n') {
                                len[y] = indx - 1;
                                y = getcury(text_win);
                                if (y == maxy - 1)
                                        mx = 1;
                                ++curnum;
                                indx = 0;
                        }
                        ++indx;
                }
                fclose(fp);
        }

        mx = -1;
        wmove(text_win, 0, 0);
        while (1) {
                wget_wch(text_win, (wint_t*)s);
                getyx(text_win, y, x);
                switch (s[0]) {
                case KEY_DOWN:
                        if (y + ofy > curnum) // do not scroll indefinetly
                                break;
                        if (y == (maxy - 1) && y + ofy < curnum) {
                                ofy++;
                                wscrl(text_win, 1);
                                wscrl(ln_win, 1);
                                mvwprintw(ln_win, maxy - 1, 0, "%2d", y + ofy + 1);
                                wrefresh(ln_win);
                                wmove(text_win, y, 0);
                                for (const wchar_t c : text[y+ofy])
                                        if (c != '\n' && 
                                            getcurx(text_win) < maxx - 1) {
                                                s[0] = c;
                                                wprintw(text_win, "%ls", s);
                                        
                                        }
                                wmove(text_win, y, x);
                        } if (mx != -1) { // go down an go to previous mx
                                if (len[y] > len[y + 1])
                                        wmove(text_win, y + 1, len[y + ofy + 1]);
                                else
                                        wmove(text_win, y + 1, x);
                        } else {
                                wmove(text_win, y + 1, x);
                        }
                        break;

                case KEY_UP:
                        if (y == 0 && ofy != 0) {
                                wscrl(text_win, -1); // scroll up
                                wscrl(ln_win, -1);
                                mvwprintw(ln_win, 0, 0, "%2d", y + ofy);
                                wrefresh(ln_win);
                                --ofy;
                                wmove(text_win, 0, 0);
                                for (const char c : text[y + ofy])
                                        if (getcurx(text_win) < maxx - 1) {
                                                s[0] = c;
                                                wprintw(text_win, "%ls", s);
                                }
                                wmove(text_win, 0, x);
                        } if (len[y + ofy] > len[y - 1 + ofy]) {
                                wmove(text_win, y - 1, len[y + ofy + 1]);
                                mx = getcurx(text_win);
                        } else {
                                wmove(text_win, y - 1, x);
                        }
                        break;

                case KEY_LEFT:
                        if (x > 0)
                                wmove(text_win, y, x - 1);
                        else
                                wmove(text_win, y - 1, len[y + ofy - 1]);
                        break;

                case KEY_RIGHT:
                        if ((size_t)x < len[y + ofy])
                                wmove(text_win, y, x + 1);
                        else if (y + ofy < curnum)
                                wmove(text_win, y + 1, 0);
                        break;

                case KEY_BACKSPACE:
                        mvwdelch(text_win, y, x - 1);
                        len[y + ofy]--;
                        break;

                case KEY_DC:
                        wdelch(text_win);
                        len[y + ofy]--;
                        break;

                case KEY_ENTER:
                        it = text[y + ofy].begin();
                        advance(it, x); 
                        len[y + ofy] -= len[y + ofy] - x;
                        wclrtoeol(text_win);
                        wmove(text_win, y + 1, 0);
                        for (const wchar_t c : text[y + ofy])
                                if (getcurx(text_win) < maxx - 1 ||
                                    getcurx(text_win))
                                        waddch(text_win, c);
                        break;

                case ctrl('R'):
                        wclear(text_win);
                        text.clear();
                        wmove(text_win, 0, 0);
                        break;

                case ctrl('A'):
                        wmove(text_win, y, 0);
                        break;

                case ctrl('E'):
                        getmaxyx(text_win, maxy, maxx);
                        --maxx;
                        wmove(text_win, y, maxx);
                        while ((winch(text_win) & A_CHARTEXT) == ' ')
                           wmove(text_win, y, --maxx);
                        wmove(text_win, y, ++maxx);
                        break;

                case ctrl('S'):
                        i = 0;
                        if (strlen(filename) != 0) {
                                fo = fopen(filename, "w");
                        } else {
                                wclear(header_win);
                                wmove(header_win, 0, 0);
                                waddstr(header_win, "Enter filename: ");
                                wrefresh(header_win);
                                wmove(header_win, 0, 15);
                                //wscanw(header_win, "%s", filename);
                                if (wgetnstr(header_win, filename, 
                                             FILENAME_MAX) == ERR
                                    || strlen(filename) != 0) {
                                        print_header(header_win, maxx);
                                        print2header(header_win, maxx, "ERROR");
                                        wmove(text_win, y, x);
                                        break;
                                }
                                fo = fopen(filename, "w");
                        }

                        while (i++ <= curnum)
                                for (const char c : text[i])
                                        fputc(c, fo);

                        fclose(fo);

                        print_header(header_win, maxx);
                        print2header(header_win, maxx, "Saved");
                        wmove(text_win, y, x);
                        break;

                case ctrl('C'):
                        delwin(text_win);
                        delwin(ln_win);
                        delwin(header_win);
                        endwin();
                        return 0;

                default:
                        //wprintw(text_win, "%c", ch);
                        wins_nwstr(text_win, s, 1);
                        wmove(text_win, y, ++x);

                        it = text[y + ofy].begin();
                        advance(it, x);
                        text[y + ofy].insert(it, (char)ch);
                        len[y + ofy]++;
                        break;
                }
        }
        //*/
        delwin(text_win);
        delwin(ln_win);
        delwin(header_win);
        endwin();
        return 0;
}
