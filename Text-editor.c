#include <stdio.h>
#include <stdlib.h>

int main()

{
    //declaring
    char txt[30];
    FILE* fp;
    int m;

    printf("\nChoose between reading-1 writing-2: ");
    scanf("%d",&m);
    printf("%d\n",m);

    if (m == 2)
    {
    printf("Instructions below:\nuse_for space\nuse up to 30 characters\n\nEnter text: ");
    scanf("%s", txt); //sets char-txt to the entered text
    printf("%s\n", txt);
        //creates file
    fp = fopen ("text.txt", "w");
    fp = fopen("text.txt", "w+");
        fputs(txt, fp);
        fclose(fp); //closes file
    }

    else
    {
        printf("text contains:\n");
        fp = fopen("text.txt","r");

        fscanf(fp,"%s",txt);
        printf("%s",txt);
        fclose(fp);

    }
}
