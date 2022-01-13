#include <stdio.h>
#include <stdlib.h>

int main()

{
    //declaring
    char txt[30];
    FILE* fp;
    int m;
    char fname[12];
    char wname[12];

    printf("Text editor made by ikozyris\nChoose between reading-1 writing-2: ");
    scanf("%d",&m);
    printf("%d\n",m);

    if (m == 2)
    {
    printf("Instructions below:\nuse_for space\nuse up to 30 characters\n\nEnter text: ");
    scanf("%s", txt); //sets char-txt to the entered text
    printf("%s\n", txt);
        //naming file
    printf("Enter file name to write: ");
        scanf("%s", &wname);
        //creates file
    fp = fopen (wname, "w");
    fp = fopen(wname, "w+");
        fputs(txt, fp);
        fclose(fp); //closes file
    }

    else
    {
        printf("Enter file name to read: ");
        scanf("%s", &fname); //naming the file to read
        printf("text contains:\n"); //listing the contents of th file
        fp = fopen(fname,"r");

        fscanf(fp,"%s",txt);
        printf("%s",txt);
        fclose(fp); //closing file

    }
}
