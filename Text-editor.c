/*
    Copyright (C) 2022  Ioannis Kozyris
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>


int main()
{
    //declaring
    
    FILE* fp;
    int m, i, lines=0, index=0;
    char fname[20],ch, txt[100];

    printf("Text editor made by ikozyris and gkozyris");
    while(1)
    {
////////MODE-SELECTOR////////////////////
        printf("\nChoose between reading-1 overwriting-2 writing-3 quit-4: ");
        scanf("%d",&m);
        while ((getchar()) != '\n');
        ch = getchar();

///////READING/////////////////////////////////
        if (m == 1)
        {
            printf("Enter file name to read: ");
            scanf("%19s", fname);
            fp = fopen(fname,"r");
            if(fp  == NULL)
            {
                fclose(fp);
                return 0;
            }

            else
            {
                while((ch=fgetc(fp))!=EOF)//getting number of lines
                {
                    if(ch=='\n')
                    lines++;
                }
                fclose(fp);
                fp = fopen(fname,"r"); //open file in read mode
                printf("\nlines:%d\n", lines);
                printf("text contains:\n%s\n",txt);
                for (i = 2; i < lines; i++)
                {
                    while ((ch = getc(fp)) != EOF)
                    {
                        putc(ch, stdout);
                        printf("%s",txt);
                    }
                }
                fclose(fp);
            }
        }

//////OVERWRITING/////////////////////////////////
        else if (m == 2)
        {
            printf("Instructions below:\nUse up to 100 characters\nUse up to 18 characters for file name\nTo exit and save press ';'\n\nEnter text: ");
            while((ch = getchar()) != '`')//reads until '`'
            {
                txt[index++] = ch;
            }
            txt[index] = '\0';
            printf("Enter file name to write: ");
            scanf("%19s", fname); //naming the file and allowing up to 19 chars
            fp = fopen(fname, "w");
            fprintf(fp, "txt = %s", txt); //creates file
            fclose(fp); //closes file
        }

////////WRITING/////////////////////////////////
        else if (m == 3)
        {
            printf("Instructions below:\nUse up to 100 characters\nUse up to 18 characters for file name\nTo exit and save press ';'\n\nEnter text: ");
            while((ch = getchar()) != '`')// reads until '`'
            {
                txt[index++] = ch;
            }
            txt[index] = '\0';
            printf("Enter file name to write: ");
            scanf("%19s", fname); //naming the file and allowing up to 19 chars
            fp = fopen(fname, "a");
            fprintf(fp, "txt = %s", txt); //creates file
            fclose(fp); //closes file
        }
////////EXITING/////////////////////////////////
        else if (m == 4)
            return 0;

        else
        {
            printf("Invalid option: %d :( use 1 or 2 or 3 or 4\n", m);
        }
    }
}
