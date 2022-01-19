#include <stdio.h>
#include <stdlib.h>

int main()

{
    //declaring
    char txt[30];
    FILE* fp;
    int m, n;
    char fname[12];
    char wname[12];


    printf("Text editor made by IKozy-2\nChoose between reading-1 overwriting-2 writing-3: ");
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
        fp = fopen(wname, "w+");
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
        fscanf(fp,"%s",txt);
        printf("%s\n",txt);
        fclose(fp);
        return 0;
    }

    else if (m == 3)
    {
        printf("Instructions below:\nuse_for space\nuse up to 30 characters\n\nEnter text: ");
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
        printf("Invalid option use 1 or 2 or 3\n");
        return 0;
    }
}
