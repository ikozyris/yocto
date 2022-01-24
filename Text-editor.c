#include <stdio.h>
#include <stdlib.h>

int main()

{
    //declaring
    char txt[30];
    FILE* fp;
    int m, n;
    char fname[12];


    printf("Text editor made by ikozyris and gkozyris\nChoose between reading-1 overwriting-2 writing-3: ");
    scanf("%d",&m);
    printf("%d\n",m);

    if (m == 2)
    {
        printf("Instructions below:\nUse_for space\nUse up to 30 characters\nUse up to 12 characters for file name\n\nEnter text: ");
        scanf("%s", txt); //sets char-txt to the entered text
        printf("%s\n", txt);
        //naming file
        printf("Enter file name to write: ");
        scanf("%s", &fname);
        //creates file
        fp = fopen(fname, "w+");
        fputs(txt, fp); //creates file
        fclose(fp); //closes file
        return 0;
    }

    else if (m == 1)
    {
        printf("Enter file name to read: ");
        scanf("%s", &fname);
        printf("text contains:\n");
        fp = fopen(fname,"r");
        fscanf(fp,"%s",txt); //reading file content
        printf("%s\n",txt);
        fclose(fp); //closing file
        return 0;
    }

    else if (m == 3)
    {
        printf("Instructions below:\nUse_for space\nUse up to 30 characters\nUse up to 12 characters for file name\n\nEnter text: ");
        scanf("%s", txt); //sets char-txt to the entered text
            //naming file
        printf("Enter file name to write: ");
        scanf("%s", &fname);
        //processing file
        fp = fopen(fname,"a"); //opens file in write mode
        fprintf(fp,"%s",txt); //writes to file without overwriting
        fclose(fp); //closes file
        printf("%s\n", txt,"%s",n);
        return 0;
    }

    else
    {
        printf("Invalid option :( use 1 or 2 or 3\n");
        return 0;
    }
}
