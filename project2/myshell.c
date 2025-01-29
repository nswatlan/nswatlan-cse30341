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
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>
#include <sys/stat.h>

#define MAX_LINE 1024
#define MAX_WORDS 128

//prototypes
void display_dir_contents(const char *dir_path); //modified code from project 1
void display_file_details(const char *file_path, const struct dirent *entry);
void display_header(const char *dir_path);
int check_if_command(char *word);  
void call_list();
void call_list_directory(char *path); 
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
    char line[MAX_LINE];
    char *words[MAX_WORDS]; 
    char *new_word; 
    int nwords = 0; 
    while(1) {
        printf("myshell> "); //may need to fix see hint (1) 
        fgets(line, MAX_LINE, stdin);

        nwords = 0; 
        memset(words, 0, sizeof(words));

        new_word = strtok(line, " \t\n"); 
        while(new_word) { //will stop at end of file (when fgets returns null)
            words[nwords] = new_word; 
            new_word = strtok(NULL, " \t\n"); 
            nwords++; 
        }
        words[nwords] = 0; 
        int num_commands = 0;
        int check;  
        while(words[num_commands] != 0) {
            //first one hardcode 0 

            //then use num_commands
            if (strcmp(words[num_commands], "list") == 0) { 
                if (words[num_commands + 1] == 0){ //means run list 
                    call_list(); 
                }
                else { //means run list on directory given  
                    check = check_if_command(words[num_commands + 1]); 
                    if (check == 0) {
                        call_list_directory(words[num_commands + 1]); 
                    }
                    if (check == 1) {
                        //not sure...perhaps c equivalent of "pass"??
                    }
                }
            } 
            if (strcmp("chdir", words[num_commands]) == 0) {
                //call function
            }
            if (strcmp("pwd", words[num_commands]) == 0) {
                call_pwd(); 
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
            num_commands++; 
        }
        
    }
    return 0; 
}

int check_if_command(char *arg){
    if (strcmp(arg, "list") == 0 || strcmp(arg, "chdir") == 0 || strcmp(arg, "pwd") == 0 
    || strcmp(arg, "start") == 0 || strcmp(arg, "wait") == 0 || strcmp(arg, "waitfor") == 0 
    || strcmp(arg, "kill") == 0 || strcmp(arg, "run") == 0 || strcmp(arg, "array") == 0){
        return 1; //indicates it is a command
    }
    else {
        return 0; //indicates it is an argument not a command
    } 

}

//function definitions
void call_list() { //lists attributes of current directory
    char cwd[1024]; 
    getcwd(cwd, sizeof(cwd));
    display_dir_contents(cwd); 
    printf("\n"); 
}

void call_list_directory(char *path) { //list attributes of given directory
    display_dir_contents(path); 
    printf("\n"); 
}

void call_chdir(){
    

}

void call_pwd(){
    char cwd[1024]; 
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd); 
}







void display_dir_contents(const char *dir_path) {
    DIR *dir;
    struct dirent *entry;
    struct stat status;
    int file_count = 0;
    int dir_count = 0;
    int symlink_count = 0;

    //try to open the directory
    dir = opendir(dir_path);
    if (!dir) {
        printf("Error: Cannot open directory %s: %s\n", dir_path, strerror(errno));
        return;
    }

    //display header info
    printf("---------------------------------------------------------\n");
    printf("NAME                      SIZE       TYPE   MODE   OWNER\n"); 
    printf("---------------------------------------------------------\n");

    //loop through the directory entries
    while ((entry = readdir(dir)) != NULL) {
        char entry_path[BUFSIZ];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", dir_path, entry->d_name);
        
        //try to get the file information
        if (lstat(entry_path, &status) == -1) {
            printf("Error: Cannot stat %s: %s\n", 
                    entry_path, strerror(errno));
            continue;
        }

        //display details of the file
        display_file_details(entry_path, entry);
    }

    //check for error with readdir
    if (errno != 0) {
        printf("Error: Cannot read directory entries - %s\n", strerror(errno));
    }

    if (closedir(dir) == -1) {
        printf("Error: Cannot close directory - %s\n", strerror(errno));
    }
}
void display_file_details(const char *file_path, const struct dirent *entry) {
    struct stat file_status;
    char owner_user[BUFSIZ] = "unknown"; 
    struct passwd *user_info;
    char file_contents[BUFSIZ] = ""; 
    char buffer[BUFSIZ] = ""; 
    int fp; //file
    char *file_type; //for printing type at end of function

    //get file details
    if (lstat(file_path, &file_status) == -1) {
        fprintf(stderr, "Error: Cannot stat %s: %s\n", file_path, strerror(errno));
        return;
    }

    //get owner name
    user_info = getpwuid(file_status.st_uid);
    if (user_info) {
        if (strncpy(owner_user, user_info->pw_name, sizeof(owner_user) - 1) == NULL) {
            fprintf(stderr, "Error: cannot username for %s\n", file_path);
            return;
        }
        owner_user[sizeof(owner_user) - 1] = '\0';
    }

    //set file types 
    if (S_ISREG(file_status.st_mode)) {
        file_type = "file";
    } else if (S_ISDIR(file_status.st_mode)) {
        file_type = "dir";
    } else if (S_ISLNK(file_status.st_mode)) {
        file_type = "link";
    } else {
        file_type = "unknown";
    }

    //display specific entry details 
    printf("%-25s %-7lldB   %-6s %04o   %-8s \n", entry->d_name, (long long)file_status.st_size, file_type, file_status.st_mode & 0777, owner_user);

}



