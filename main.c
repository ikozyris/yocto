#include <ncurses.h>
#include <string.h>

#define ctrl(x)           ((x) & 0x1f)

int main() {
    int y, x;
    int row,col, row1, colmid;				// to store the number of rows and columns
    int ch, i, crow;
    FILE *fp;

    // initialize curses
    initscr();
    keypad(stdscr, TRUE);

    getmaxyx(stdscr,row,col);		// get the number of rows and columns
    row1 = row/row -1;
    colmid = col/2 -10;
    attron(A_STANDOUT);
    for (i = 0;  i < col; i++)
        printw(" ");
    mvprintw(row1,colmid,"Yocto 0.8-alpha1");
    attroff(A_STANDOUT);
    wmove(stdscr,1, 0);
    getyx(stdscr,x,y);

    ////READING///
    /*
    fp = fopen("main.c","r");
    crow = row;
    while((ch = fgetc(fp)) != EOF)	// read the file till we reach the end 
    {
        getyx(stdscr, y, x);		// get the current curser position
        if(y == (row - 1))			// are we are at the end of the screen 
        {
            crow
            printw("<-Press Any Key->");	// tell the user to press a key 
            wgetch(stdscr);
            wmove(stdscr, crow/, 0);			// start at the beginning of the screen 

        }
        printw("%c", ch);
    }
    */
    ////END//OF//READING////

    while(ch = getch()) {
        if(ch == KEY_DOWN) {
            getyx(stdscr,x,y);
            x=x+1;
            wmove(stdscr,x, y);
            break;
        }
        if(ch == KEY_UP) {
            getyx(stdscr,x,y);
            x=x-1;
            wmove(stdscr,x, y);
            break;
        }
        if(ch == KEY_LEFT) {
            getyx(stdscr,x,y);
            y=y-1;
            wmove(stdscr,x, y);
            break;
        }
        if(ch == KEY_RIGHT) {
            getyx(stdscr,x,y);
            y=y+1;
            wmove(stdscr,x, y);
            break;
        }
        if(ch == KEY_BACKSPACE) {
            getyx(stdscr,x,y);
            delch();
            break;
        }
        if(ch == 10) { //enter
            getyx(stdscr,x,y);
            wmove(stdscr,x+1,0);
            break;
        }
        if(ch == ctrl('X')) {
            break;
        }
        if(ch == ctrl('C')) {
                endwin();
                return 0;
                break;
        }
    }
    endwin();
    refresh();
    return 0;
}
