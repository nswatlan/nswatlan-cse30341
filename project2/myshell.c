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
    char line[MAX];
    char *words[MAX]; 
    char *new_word; 
    int nwords = 0; 
    while(1) {
        printf("myshell> "); //may need to fix see hint (1) 
        fgets(line, MAX, stdin); 
        new_word = strtok(line, " \t\n"); 
        while(new_word) {
            words[nwords] = new_word; 
            new_word = strtok(NULL, " \t\n"); 
            nwords++; 
        }
        words[nwords] = 0; 
        for (int i = 0; i < nwords; i++){
            printf("%s\n",words[i]); 
        }
        printf("nwords %d", nwords);
        break; 
    }



    return 0; 
}



//function definitions


