#define ctrl(x)         ((x) & 0x1f)

#undef 	KEY_ENTER
#define KEY_ENTER       10 // overwrite default

/* EDIT BELOW to change keybindings */

#define ENTER		KEY_ENTER
#define SAVE		ctrl('S')
#define EXIT		ctrl('C')
#define UP		KEY_UP
#define DOWN		KEY_DOWN
#define LEFT		KEY_LEFT
#define RIGHT		KEY_RIGHT
#define BACKSPACE	KEY_BACKSPACE
#define DELETE		KEY_DC
#define HOME		ctrl('A')
#define END		ctrl('E')
#define INFO		ctrl('I')
#define REFRESH		ctrl('R')
