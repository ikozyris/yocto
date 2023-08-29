/*
    Copyright (C) 2022-2023  ikozyris
    Copyright (C) 2022-2023  gkozyris
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
    int ch, indx, m;
    char txt[2001], fname[51];

    while(1) {
////////MODE-SELECTOR////////////////////
        printf("Enter a digit from: reading->1 overwriting->2 appending->3 quit->4: ");
        scanf("%1d", &m);

        while ((getchar()) != '\n'); // clear stdin buffer

///////READING/////////////////////////////////
        if (m == 1) {
            lines = 1;
            printf("Enter file name to read: ");
            scanf("%50s", fname);
            fp = fopen(fname,"r");
            if(fp  == NULL) {
                perror("fopen(): ");
                return -1;
            }
            
            printf ("%03zu| ", lines++);
            while ((ch = getc(fp)) != EOF) {
                putchar (ch);
                if (ch == '\n')              
                    printf ("%03zu| ", lines++); //print line number with 001 format
            }
            fclose(fp);
            printf("\n");
        }
    
//////OVERWRITING/////////////////////////////////
        else if (m == 2) {
            indx = 0;
            printf("Instructions below:\nUse up to 2000 characters\nUse up to 50 characters for file name\n"
                   "To exit and save press < and ENTER\nEnter text:\n");
            while((ch = getchar()) != '<') { //reads until <
                txt[indx++] = ch;
                if (indx == 1999) break;
            }
            txt[indx] = '\0';
            printf("Enter file name to write: ");
            scanf("%50s", fname); //naming the file and allowing up to 50 chars
            fp = fopen(fname, "w");
            fprintf(fp, "%s", txt); //writes file
            fclose(fp); //closes file
        }

////////WRITING/////////////////////////////////
        else if (m == 3) {
            indx = 0;
            printf("Instructions below:\nUse up to 2000 characters\nUse up to 50 characters for file name\n"
                   "To exit and save press < and ENTER\nEnter text:\n");
            while((ch = getchar()) != '<') {// reads until '<'
                txt[indx++] = ch;
                if (indx == 1999) break; // leave one cell for EOF
            }
            txt[indx] = '\0';
            printf("Enter file name to write: ");
            scanf("%50s", fname); //naming the file and allowing up to 50 chars
            fp = fopen(fname, "a");
            fprintf(fp, "%s", txt); //writes file
            fclose(fp); //closes file
        }
    
        else break;
    } //end of main while loop
    
    return 0;
}
