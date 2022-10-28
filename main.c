#include <ncurses.h>
#include <string.h>

#define ctrl(x)           ((x) & 0x1f)

int main() {
    int y, x;
    int maxy,maxx;				// to store the number of rows and columns
    int ch, i, crow;
    char buffer[500];

    FILE *fp;

    // initialize curses
    initscr();
    keypad(stdscr, TRUE);

    getmaxyx(stdscr,maxy,maxx);		// get the number of rows and columns
    attron(A_STANDOUT);
    for (i = 0;  i < maxx; i++)
        printw(" ");
    mvprintw(0, maxx/2 -10,"Yocto 0.8-alpha1");
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

    while((ch = getch())) {
        if(ch == KEY_DOWN) {
            getyx(stdscr,x,y);
            x=x+1;
            wmove(stdscr,x, y);
        }
        if(ch == KEY_UP) {
            getyx(stdscr,x,y);
            x=x-1;
            wmove(stdscr,x, y);
        }
        if(ch == KEY_LEFT) {
            getyx(stdscr,x,y);
            y=y-1;
            wmove(stdscr,x, y);
        }
        if(ch == KEY_RIGHT) {
            getyx(stdscr,x,y);
            y=y+1;
            wmove(stdscr,x, y);
        }
        if(ch == KEY_BACKSPACE) {
            getyx(stdscr,x,y);
            delch();
        }
        if(ch == 10) { //enter
            getyx(stdscr,x,y);
            wmove(stdscr,x+1,0);
        }
        if(ch == ctrl('A')) {
            getyx(stdscr,x,y);
            wmove(stdscr,x,0);
        }
        if(ch == ctrl('S')) {
            mvscanw(1,maxx,"%s",&buffer);
            fp = fopen("text.txt","w");
            mvprintw(1,maxx - 6, "Saved");
            fprintf(fp, "%s", buffer);
        }

        if(ch == ctrl('C')) {
            endwin();
            return 0;
        }
    }
    endwin();
    return 0;
}
