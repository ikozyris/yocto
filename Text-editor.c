#include <stdio.h>

int main()
{
    //declaring
    char txt[30];
    FILE* fp;
    int m, i, lines=0;
    char fname[12],ch;

    printf("Text editor made by ikozyris and gkozyris");
    while(1)
    {
        printf("\nChoose between reading-1 overwriting-2 writing-3 quit-4: ");
        scanf("%d",&m);

        if (m == 2)
        {
            printf("Instructions below:\nUse_for space\nUse up to 30 characters\nUse up to 12 characters for file name\n\nEnter text: ");
            scanf("%s", txt); //sets char-txt to the entered text
            printf("%s\n", txt);
            printf("Enter file name to write: ");
            scanf("%s", &fname); //naming the file
            fp = fopen(fname, "w");
            fputs(txt, fp); //creates file
            fclose(fp); //closes file
        }

        else if (m == 1)
        {
            printf("Enter file name to read: ");
            scanf("%s", &fname);
            fp = fopen(fname,"r");

            while((ch=fgetc(fp))!=EOF)//getting number of lines (works)
            {
                if(ch=='\n')
                lines++;
            }
            fclose(fp);
            fp = fopen(fname,"r");
            printf("\nlines:%d\n", lines);
            fscanf(fp,"%[^\n]",txt);
            printf("text contains:\n%s\n",txt);
            for (i = 1; i < lines; i++)
            {
                fscanf(fp,"\n%[^\n]\n",txt);
                printf("%s\n",txt);
            }
            fclose(fp);
        }

        else if (m == 3)
        {
            printf("Instructions below:\nUse_for space\nUse up to 30 characters for file content\nUse up to 12 characters for file name\n\nEnter text: ");
            scanf("%s", txt); //sets char-txt to the entered text
                //naming file
            printf("Enter file name to write: ");
            scanf("%s", &fname);
                //processing file
            fp = fopen(fname,"a"); //opens file in write mode and enters text in the end of the line
            fprintf(fp,"%s",txt); //writes to file
            fclose(fp); //closes file
            printf("%s\n", txt);
        }
        else if (m == 4)
            return 0;

        else
        {
            printf("Invalid option: %d :( use 1 or 2 or 3 or 4\n", m);
        }
    }
}
