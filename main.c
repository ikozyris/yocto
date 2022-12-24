#include <ncurses.h>
//#include <string.h>
//#include <unistd.h>
//#include <locale.h>

#define ctrl(x)           ((x) & 0x1f)

int main() {
    int y, x;
    int maxy,maxx; // to store the maximum rows and columns
    int ch, i, cha;
    char buffer[2000];

    FILE *fp;

    // initialize curses
    initscr();
    //cbreak();
    keypad(stdscr, TRUE);
    raw();
    //scrollok(stdscr, TRUE);
    noecho();

    //setlocale(LC_ALL, ""); //for reading non-ascii

    getmaxyx(stdscr,maxy,maxx);	// get the number of rows and columns
    attron(A_STANDOUT);
    for (i = 0;  i < maxx; i++)
        printw(" ");
    mvprintw(0, maxx/2 -10,"Yocto 0.8-alpha1");
    attroff(A_STANDOUT);
    move(1, 0);
/*
    ////READING///
    fp = fopen("main.c","r");
    while((cha = fgetc(fp)) != EOF)	// read the file till we reach the end 
    {
        getyx(stdscr, y, x);		// get the current curser position
        printw("%c", cha);
    }
    ////END//OF//READING////*/
    getmaxyx(stdscr,maxy,maxx);
    while((ch = getch())) {
        if(ch == KEY_DOWN) {
            getyx(stdscr,y,x);
            if(y == (maxy - 1))	{ // are we are at the end of the screen
                move(1,0);
                clrtoeol();
                move(1, x);
            }
            move(y+1, x);
        }
        else if(ch == KEY_UP) {
            getyx(stdscr,y,x);
            if(y!=1)
                move(y-1, x);
        }
        else if(ch == KEY_LEFT) {
            getyx(stdscr,y,x);
            move(y, x-1);
        }
        else if(ch == KEY_RIGHT) {
            getyx(stdscr,y,x);
            move(y, x+1);
        }
        else if(ch == KEY_BACKSPACE) {
            getyx(stdscr,y,x);
            move(y,x-1);
            delch();
        }
        else if(ch == KEY_DC) {
            delch();
        }
        else if(ch == 10) { //enter
            getyx(stdscr,y,x);
            move(y+1,0);
        }
        else if(ch == ctrl('R')) {
            move(1,0);
        }
        else if(ch == ctrl('A')) {
            getyx(stdscr,y,x);
            move(y,0);
        }
        else if(ch == ctrl('E')) {
            getyx(stdscr,y,x);
            getmaxyx(stdscr,maxy,maxx);
            maxx--;
	        move(y, maxx);
            while((inch() &  A_CHARTEXT) == ' ') {
               move(y, --maxx);
            }
            move(y,++maxx);
        }
        else if(ch == ctrl('S')) {
            mvscanw(1,maxx,"%s",&buffer);
            fp = fopen("text.txt","w");
            fprintf(fp, "%s", buffer);
            fclose(fp);
	        getyx(stdscr,y,x);
            attron(A_STANDOUT);
            mvprintw(0, maxx -6,"Saved ");
            attroff(A_STANDOUT);
	        move(y,x);
        }
        else if(ch == ctrl('C')) {
            endwin();
            return 0;
        }
        else printw("%c",ch);
    }
    endwin();
    return 0;
}
