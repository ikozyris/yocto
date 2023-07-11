#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ctrl(x)           ((x) & 0x1f)

int main(int argc, char *argv[])
{
    bool max;
    int y = 0, x = 0;
    int ofy = 0; // offsets: file<->screen y
    int maxy = 0, maxx = 0; // to store the maximum rows and columns
    int ch, i;
    char text[300][300]; //the file content
    //text = (char*[])malloc(300 * sizeof(char*));
    //for (i = 0; i < 300; ++i)
    //    text[i] = NULL;
    char buffer[300];
    unsigned len[300]; // lenght of text[x] in line x

    // initialize curses
    initscr();
    getmaxyx(stdscr, maxy, maxx);	// get the number of rows and columns

    //cbreak();
    raw();
    noecho();

    scrollok(stdscr, TRUE);
    keypad(stdscr, TRUE);

    wmove(stdscr, 0, 0);
    int index = 0, curnum = 0;
    FILE *fp;
    if (argc > 1) {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            perror("fopen");
            return -1;
        }
        while((ch = fgetc(fp)) != EOF) { // read the file till we reach the end
            //printw("%c", ch);
            buffer[index] = ch;
            text[curnum][index] = ch;
            ++index;
            if (ch == '\n') {
                //text[curnum] = buffer;
                len[curnum] = index - 1;
                y = getcury(stdscr);
                if (y + 1 < maxy)
                    printw("%s", text[curnum]);
                //printw("%d|%d: %s", curnum, index, text[curnum]);
                curnum++;
                index = 0;
                bzero(buffer, 299);
            }
        }
        fclose(fp);
    }
    refresh();

    while ((ch = getch())) {
        getyx(stdscr, y, x);
        refresh();
        switch (ch) {
            case KEY_DOWN:
                if (y == (maxy - 1) && y + ofy < curnum) { // Are we are at the end of the screen
                    ofy++;
                    scroll(stdscr);
                    mvprintw(maxy-1, 0, "%s", text[y + ofy]);
                } if (len[y] > len[y+1]) {
                    max = true;
                    move(y+1, len[y+1]);
                } else {
                    move(y+1, x);
                }
                y++;
                break;

            case KEY_UP:
                if (y == 0 && text[y + ofy] != NULL && ofy != 0) {
                    scrl(-1); // scroll up
                    ofy--;
                    mvprintw(0, 0, "%s", text[y + ofy]);
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
                break;

            case KEY_DC:
                delch();
                break;

            case 10: //enter
                wmove(stdscr, y+1, 0);
                break;

            case ctrl('R'):
                wmove(stdscr, 0, 0);
                clear();
                ofy = 0;
                for (i = 0; i < curnum; i++) {
                    addstr(text[i]);
                    ofy++;
                    refresh();
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
                //putwin(stdscr, fp2);
                fclose(fp2);
                break;

            case ctrl('C'):
                endwin();
                return 0;

            default:
                insch(ch);
                text[y+ofy][x] = ch;
                break;
        }
    }
    fclose(fp);
    endwin();
    return 0;
}
