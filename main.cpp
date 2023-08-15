#include <ncurses.h>
#include <cstdlib>
#include <cstring>
//#include <locale.h>
#include <list>
#include <vector>
#include <array>
#include <unistd.h>

#define ctrl(x)         ((x) & 0x1f)
#define MAX_LINES       5000

using namespace std;

vector<list<char>> text(MAX_LINES);
size_t len[MAX_LINES]; //lenght of each line

int y = 0, x = 0;
// offset in y axis of text and screen 
signed int ofy = 0;
int maxy = 0, maxx = 0; // to store the maximum rows and columns
int ch, i;
const char *name = "Yocto 0.8-alpha1";

// indx: tmp for lenght of line
// curnum: total lines
int indx = 0, curnum = 0, mx = -1;

int main(int argc, char *argv[]) {
        for (int i = MAX_LINES - 1; i > 0; --i) {
                len[i] = 0;
        }

        // initialize curses
        initscr();
        getmaxyx(stdscr, maxy, maxx);	// get the number of rows and columns
        cbreak();
        raw();
        noecho();
        refresh();

        // initialize header window
        WINDOW *header_win;
        header_win = newwin(1, maxx, 0, 0);
        wattrset(header_win, A_STANDOUT);
        wmove(header_win, 0, 0);
        for (i = 0;  i < maxx; ++i)
                wprintw(header_win, " ");
        mvwprintw(header_win, 0, maxx / 2 - strlen(name) / 2, "%s", name);
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

        getmaxyx(text_win, maxy, maxx);	// get the number of rows and columns

        FILE *fo;
        if (argc > 1) {
                FILE *fp;
                fp = fopen(argv[1], "r");
                if (fp == NULL) {
                        perror("fopen");
                        return -1;
                }
                while((ch = fgetc(fp)) != EOF) {
                        text[curnum].push_back(ch);
                        //wprintw(text_win, "%c", ch);
                        if (mx == -1 && indx < maxx)
                                waddch(text_win, ch);
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
        while ((ch = wgetch(text_win))) {
                getyx(text_win, y, x);
                switch (ch) {
                case KEY_DOWN:
                        if (y + ofy > curnum)
                                break;
                        if (y == (maxy - 1) && y + ofy < curnum) { //scroll down
                                ofy++;
                                wscrl(text_win, 1);
                                wscrl(ln_win, 1);
                                mvwprintw(ln_win, maxy - 1, 0, "%2d", y + ofy + 1);
                                wrefresh(ln_win);
                                wmove(text_win, y, 0);
                                for (const char x : text[y+ofy])
                                        if (x != '\n' && 
                                            getcurx(text_win) < maxx-1)
                                                waddch(text_win, x);
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
                                wscrl(text_win, - 1); // scroll up
                                wscrl(ln_win, - 1);
                                mvwprintw(ln_win, 0, 0, "%2d", y + ofy);
                                wrefresh(ln_win);
                                --ofy;
                                wmove(text_win, 0, 0);
                                for (const char x : text[y + ofy])
                                        if (getcurx(text_win) < maxx - 1)
                                                waddch(text_win, x);
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
                        if (len[y + ofy] >= (size_t)x)
                                wmove(text_win, y, x + 1);
                        else if (y + ofy < curnum)
                                wmove(text_win, y + 1, 0);
                        break;

                case KEY_BACKSPACE:
                        mvwdelch(text_win, y, x - 1);
                        break;

                case KEY_DC:
                        wdelch(text_win);
                        break;

                case 10: //enter
                        wmove(text_win, y + 1, 0);
                        break;

                case ctrl('R'):
                        wclear(text_win);
                        wmove(text_win, 0, 0);
                        break;

                case ctrl('A'):
                        wmove(text_win, y, 0);
                        break;

                case ctrl('E'):
                        getmaxyx(text_win, maxy, maxx);
                        maxx--;
                        wmove(text_win, y, maxx);
                        while ((winch(text_win) &  A_CHARTEXT) == ' ') {
                           wmove(text_win, y, --maxx);
                        }
                        wmove(text_win, y, ++maxx);
                        break;

                case ctrl('S'):
                        mvwprintw(header_win, 0, maxx - 7, "Saved");
                        wrefresh(header_win);
                        i = 0;
                        fo = fopen(argv[1], "w");
                        while (len[i] != 0) {
                                for (const char x : text[i])
                                        fputc(x, fo);
                                ++i;
                        }
                        fclose(fo);
                        break;

                case ctrl('C'):
                        delwin(text_win);
                        delwin(ln_win);
                        delwin(header_win);
                        endwin();
                        return 0;

                default:
                        //wprintw(text_win, "%c", ch);
                        winsch(text_win, ch);
                        wmove(text_win, y, ++x);

                        list<char>::iterator it = text[y + ofy].begin();
                        advance(it, 0);
                        text[y + ofy].assign(3, 'c');
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
