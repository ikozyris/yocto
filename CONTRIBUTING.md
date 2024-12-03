Follow the Linux Kernel coding style. <br>
With the exception that the maximum lenght of line is 100

The most important things are:
- Use tabs (*not* spaces), for flexibility and reduced file size
- Braces open in the same line as the if, while, switch... except functions

example:
```c
#include <stdio.h>
#include <string.h>

int main()
{
	char input[5];
ultimate_question:
	printf("Which is the best text editor? ");
	scanf("%5s", input);
	// 42 is magic number
	if (strcmp(input, "kri") == 0 || strcmp(input, "42") == 0) {
		printf("Correct!\n");
		goto exit;
	} else {
		printf("Are you sure?\n");
		scanf("%s", input);
		if (strcmp(input, "yes") == 0)
			while (1)
				printf("NO! ");
		else
			goto ultimate_question;
	}
	printf("This will never be executed!");
exit:
	return 0;
}
```

For pull/merge requests (PR/MR):
- Descriptive comments in commits, title, description
	- Use the following format for commits:<br>
```
fix: bug when something, regression | perf: x9999 boost in writing

The regression exists since [commit number]
[More optional information about commit]
```
- If the change is just a one-liner create an issue not a PR/MR
- Try to benchmark any optimizations
- Split large commits, and try make commit titles <70 characters

Make sure to check any "TODO:"