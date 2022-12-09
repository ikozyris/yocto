#include <ncurses.h>
#include <string.h>

#define ctrl(x)           ((x) & 0x1f)

int main() {
    int y, x;
    int maxy,maxx;				// to store the maximum rows and columns
    int ch, i, crow;
    char buffer[2000];

    FILE *fp;

    // initialize curses
    initscr();
    keypad(stdscr, TRUE);
    raw();

    getmaxyx(stdscr,maxy,maxx);		// get the number of rows and columns
    attron(A_STANDOUT);
    for (i = 0;  i < maxx; i++)
        printw(" ");
    mvprintw(0, maxx/2 -10,"Yocto 0.8-alpha1");
    attroff(A_STANDOUT);
    wmove(stdscr,1, 0);

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
    getmaxyx(stdscr,maxy,maxx);
    while((ch = getch())) {
        if(ch == KEY_DOWN) {
            getyx(stdscr,y,x);
            wmove(stdscr,y+1, x);
        }
        if(ch == KEY_UP) {
            getyx(stdscr,y,x);
            wmove(stdscr,y-1, x);
        }
        if(ch == KEY_LEFT) {
            getyx(stdscr,y,x);
            wmove(stdscr,y, x-1);
        }
        if(ch == KEY_RIGHT) {
            getyx(stdscr,y,x);
            wmove(stdscr,y, x+1);
        }
        if(ch == KEY_BACKSPACE) {
            getyx(stdscr,y,x);
            if(x == 0)
                wmove(stdscr,y-1,maxx);
            delch();
        }
        if(ch == KEY_DC) {
            getyx(stdscr,x,y);
            wmove(stdscr,y, x+1);
            delch();
            wmove(stdscr,y, x-1);
        }
        if(ch == 10) { //enter
            getyx(stdscr,y,x);
            wmove(stdscr,y+1,0);
        }
        if(ch == ctrl('R')) {
            wmove(stdscr,1,0);
        }
        if(ch == ctrl('A')) {
            getyx(stdscr,y,x);
            wmove(stdscr,y,0);
        }
        if(ch == ctrl('E')) {
            getyx(stdscr,y,x);
            wmove(stdscr,y,maxx);
        }
        if(ch == ctrl('S')) {
            mvscanw(1,maxx,"%s",&buffer);
            fp = fopen("text.txt","w");
            fprintf(fp, "%s", buffer);
            fclose(fp);
	        getyx(stdscr,y,x);
            attron(A_STANDOUT);
            mvprintw(0, maxx -6,"Saved ");
            attroff(A_STANDOUT);
	        wmove(stdscr,y,x);

        }
        if(ch == ctrl('C')) {
            endwin();
            return 0;
        }
    }
    endwin();
    return 0;
}
