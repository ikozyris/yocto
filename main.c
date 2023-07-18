#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ctrl(x)           ((x) & 0x1f)

// TODO: do not hardcode these, but malloc according to file size
#define MAX_LINES 2000
#define MAX_LENGHT 300

// Inserts a character at a specicified position without overwriting
// USAGE: size of array, array, character to insert, index to insert to
char* insert(int n, char arr[], char x, int pos)
{
    // shift elements forward
    for (int i = n + 1; i >= pos; --i)
        arr[i] = arr[i - 1];
 
    // insert x at pos
    arr[pos - 1] = x;
 
    return arr;
}

// Deletes the character at a specicified position and shifts elements back
// USAGE: size of array, array, index to delete
char *delIndx(int n, char arr[], int pos)
{
    for (int i = pos; i <= n - 1; ++i)
        arr[i] = arr[i + 1];

    return arr;
}

int main(int argc, char *argv[])
{
    bool max = false;
    int y = 0, x = 0;
    signed int ofy = 0; // offsets: file<->screen y
    int maxy = 0, maxx = 0; // to store the maximum rows and columns
    int ch, i;
    // char ** and malloc do not have the same result as 2d array  
    char text[MAX_LINES][MAX_LENGHT]; //the file content
    unsigned int len[MAX_LINES]; // lenght of text[x] in line x

    // initialize curses
    initscr();
    getmaxyx(stdscr, maxy, maxx);	// get the number of rows and columns

    //cbreak();
    raw();
    noecho();

    keypad(stdscr, TRUE);

    move(0, 0);
    int index = 0, curnum = 0;
    FILE *fp;

    if (argc > 1) {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            perror("fopen");
            return -1;
        }
        while ((ch = fgetc(fp)) != EOF) { // read the file till we reach the end
            text[curnum][index] = ch;
            ++index;
            if (ch == '\n') {
                len[curnum] = index - 1;
                y = getcury(stdscr);
                // print before check to avoid empty line
                if (!max)
                    printw("%.*s", maxx, text[curnum]);
                if (y == maxy - 1)
                    max = true;
                //printw("%d|%d: %s", curnum, index, text[curnum]);
                curnum++;
                index = 0;
            }
        }
    }
    refresh();
    max = false;
    move(0, 0);
    // Scrolling is enabled after printing
    scrollok(stdscr, TRUE);

    while ((ch = getch())) {
        getyx(stdscr, y, x);
        refresh();
        switch (ch) {
            case KEY_DOWN:
                if (y == (maxy - 1) && y + ofy < curnum) {
                    ofy++;
                    scroll(stdscr);
                    mvprintw(maxy - 1, 0, "%.*s", len[y + ofy], text[y + ofy]);
                } if (len[y+ofy] > len[y+ofy+1]) {
                    max = true;
                    move(y+1, len[y+1]);
                } else {
                    move(y+1, x);
                }
                break;

            case KEY_UP:
                if (y == 0 && ofy != 0) {
                    scrl(-1); // scroll up
                    ofy--;
                    mvprintw(0, 0, "%.*s", maxx, text[y + ofy]);
                    move(0, x);
                } else {
                    wmove(stdscr, y-1, x);
                }
                break;

            case KEY_LEFT:
                if (x == len[y])
                    move(y-1, 0);
                else
                    move(y, x - 1);
                break;

            case KEY_RIGHT:
                if (x == len[y])
                    move(y+1, 0);
                if (text[y] == NULL)
                    break;
                if (len[y] > x)
                    wmove(stdscr, y, x+1);
                break;

            case KEY_BACKSPACE:
                mvdelch(y, x-1);
                delIndx(len[y]+1, text[y], x-1);
                break;

            case KEY_DC:
                delch();
                delIndx(len[y]+1, text[y], x);
                break;

            case 10: //enter
                wmove(stdscr, y+1, 0);
                insert(len[y+1]+1, text[y], '\n', x);
                break;

            case ctrl('R'):
                wmove(stdscr, 0, 0);
                clear();
                ofy = 0;
                max = false;
                for (i = 0; i < curnum; i++) {
                    if (!max)
                        printw("%.*s", maxx, text[curnum]);
                    if (y == maxy - 1)
                        max = true;
                }
                break;

            case ctrl('A'):
                wmove(stdscr, y, 0);
                break;

            case ctrl('E'):
                getmaxyx(stdscr, maxy, maxx);
                maxx--;
                wmove(stdscr, y, maxx);
                while ((inch() &  A_CHARTEXT) == ' ')
                    wmove(stdscr, y, --maxx);
                wmove(stdscr, y, ++maxx);
                break;

            case ctrl('S'):
                FILE *fp2 = fopen("output.txt", "w");
                for (i = 0; i < curnum; i++)
                    fprintf(fp2, "%s", text[i]);
                fclose(fp2);
                break;

            case ctrl('C'):
                endwin();
                return 0;

            default:
                // insert, not overwrite
                insch(ch);
                move(y, ++x);
                insert(len[y]+1, text[y], ch, x);
                break;
        }
    }
    fclose(fp);
    endwin();
    return 0;
}
