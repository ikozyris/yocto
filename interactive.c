/*
    Copyright (C) 2022  ikozyris
    Copyright (C) 2022  gkozyris
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
#include <string.h>

int interactive()
{
    FILE* fp;
    size_t lines = 1;
    int ch, index, m;
    char txt[500], fname[38];
    char choice[2];

    while(1)
    {
////////MODE-SELECTOR////////////////////
        printf("Choose between reading-1 overwriting-2 writing-3 quit-4: ");
        scanf("%c",choice);

        m=atoi(choice);

        while ((getchar()) != '\n'); // clear stdin buffer

///////READING/////////////////////////////////
        if (m == 1)
        {
            memset(txt, '\0', sizeof(txt));//reset txt
            lines = 1;
            printf("Enter file name to read: ");
            scanf("%s", fname);
            fp = fopen(fname,"r");
            if(fp  == NULL)
           {
                perror("fopen(): ");
                return -1;
            }
            
            printf ("%03zu| ", lines++);
            while ((ch = getc(fp)) != EOF) 
            {
                putchar (ch);
                if (ch == '\n')              
                    printf ("%03zu| ", lines++); //print line number with 001 format
            }
            printf("\n");
        }
    
//////OVERWRITING/////////////////////////////////
        else if (m == 2)
        {
            index =0;
            printf("Instructions below:\nUse up to 100 characters\nUse up to 18 characters for file name\nTo exit and save press '`'\n\nEnter text:\n");
            while((ch = getchar()) != '`')//reads until '`'
            {
                txt[index++] = ch;
            }
            txt[index] = '\0';
            printf("Enter file name to write: ");
            scanf("%s", fname); //naming the file and allowing up to 19 chars
            fp = fopen(fname, "w");
            fprintf(fp, "%s", txt); //creates file
            fclose(fp); //closes file
        }

////////WRITING/////////////////////////////////
        else if (m == 3)
        {
            index = 0;
            printf("Instructions below:\nUse up to 100 characters\nUse up to 18 characters for file name\nTo exit and save press '`'\n\nEnter text:\n");
            while((ch = getchar()) != '`')// reads until '`'
            {
                txt[index++] = ch;
            }
            txt[index] = '\0';
            printf("Enter file name to write: ");
            scanf("%s", fname); //naming the file and allowing up to 19 chars
            fp = fopen(fname, "a");
            fprintf(fp, "%s", txt); //creates file
            fclose(fp); //closes file
        }
    
        else if (m == 4)
        return 0;
    }
    
    return 0;
}
