//Norah Swatland
//CSE 30341 Project 2 

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>

#define MAX 100
//prototypes


//main
int main() {
    char line[100]; 
    while(1) {
        printf("myshell> "); 
        fgets(line, MAX, stdin); 
        printf("%s", line); 
        break; 
    }



    return 0; 
}



//function definitions


