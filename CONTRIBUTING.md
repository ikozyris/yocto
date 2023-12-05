Follow the Linux Kernel coding style. <br>
With the exception that the maximum lenght of line is 100

The most important things are:
- Descriptive comments in commits
- Use tabs (*not* spaces)
- Braces open in the same line as the if, while, switch... except functions

example:
```c
#include <stdio.h>
#include <string.h>

int main()
{
	char input[5];
	printf("Which is the best text editor? ");
	scanf("%5s", input);
	if (strcmp(input, "yocto") == 0) {
		printf("Correct!\n");
	} else {
		while (1)
			printf("NO! ");
	}
}
```
