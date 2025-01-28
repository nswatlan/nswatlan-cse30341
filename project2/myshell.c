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
void call_list();
void call_chdir(); 
void call_pwd();
void call_start(); 
void call_wait(); 
void call_waitfor(); 
void call_kill(); 
void call_run(); 
void call_array(); 

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
        while(new_word) { //will stop at end of file (when fgets returns null)
            words[nwords] = new_word; 
            new_word = strtok(NULL, " \t\n"); 
            nwords++; 
        }
        words[nwords] = 0; 

        //display for debugging purposes
        for (int i = 0; i < nwords; i++){
            printf("words[%d] : %s\n",i, words[i]); 
        }
        printf("nwords %d", nwords);
        int num_commands = 0; 
        while(words[num_commands] != 0) {
            //first one hardcode 0 

            //then use num_commands
            if (strcmp("list", words[num_commands]) == 0) {
                //call function
            }
            if (strcmp("chdir", words[num_commands]) == 0) {
                //call function
            }
            if (strcmp("pwd", words[num_commands]) == 0) {
                //call function
            }
            if (strcmp("start", words[num_commands]) == 0) {
                //call function
            }
            if (strcmp("wait", words[num_commands]) == 0) {
                //call function
            }
            if (strcmp("waitfor", words[num_commands]) == 0) {
                //call function
            }
            if (strcmp("kill", words[num_commands]) == 0) {
                //call function
            }
            if (strcmp("run", words[num_commands]) == 0) {
                //call function
            }
            if (strcmp("array", words[num_commands]) == 0) {
                //call function
            }
        }
    }


    return 0; 
}



//function definitions


