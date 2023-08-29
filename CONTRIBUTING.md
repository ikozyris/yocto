Follow the Linux Kernel coding style. <br>
With the exception that the maximum lenght of line is 100

The most important things are:
- Descriptive comments in commits
- Use tabs with 8 width
- Braces open in the same line as the if, while, switch... except functions

example:
```c
#include <stdio.h>
#include <string.h>

int main()
{
	char input[20];
	printf("Which is the best text editor? ");
	scanf("%20s", input);
	if (strcmp(input, "yocto") == 0) {
		printf("Correct!");
	} else {
		printf("Wrong answer");
	}
}
```
