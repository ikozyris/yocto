/*
    Copyright (C) 2022  ikozyris
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

#include "interactive.c"
#include "whes.c"

int main(int argc, char *argv[])
{

    //declaring
    char option[20];


    if (argc == 1)
        interactive();
    
    if (argc == 2)
    {
        if (strcmp("--version", argv[1]) == 0) {
            printf("Yocto Version 0.7 Created by ikozyris and gkozyris\nLicensed under Gnu General Public License v3\nSource code available at: http://github.com/ikozyris/yocto/ \n");
            return 0;
        }

        if (strcmp("-i", argv[1]) == 0 || strcmp("--interactive", argv[1]) == 0) {
            interactive();
            return 0;
        }

	    if (strcmp("-h", argv[1]) == 0 || strcmp("--help", argv[1]) == 0) {
            hlp();
	        return 0;
	    }

        else printf("Invalid argument %s", argv[1]);  
    }

    if (argc == 3)
    {
	
        strcpy(fname, argv[1]);
        strcpy(option, argv[2]);

        if (strcmp("-i", option) == 0 || strcmp("--interactive", option) == 0) {
            interactive();
            return 0;
        }

        if (strcmp("-sn", option) == 0 || strcmp("--single-number", option) == 0) {
            sn();
            return 0;
        }

        if (strcmp("-e", option) == 0 || strcmp("--show-ends", option) == 0) {
            e();
            return 0;
        }

        if (strcmp("-rl", option) == 0 || strcmp("--read-lines", option) == 0) {
            rl();
            return 0;
        }

        if (strcmp("-r", option) == 0 || strcmp("--read", option) == 0) {
            r();
            return 0;
        }

        if (strcmp("-w", option) == 0 || strcmp("--write", option) == 0) {
            wrt();
            return 0;
        }

        else hlp();
    }

    if (argc > 3)
    {
        printf("Too many arguments\n");
        return 0;
    }


    return 0;
}

