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
#include <sys/wait.h>
#include <signal.h> 

#define MAX_LINE 1024
#define MAX_WORDS 128

//prototypes
void display_dir_contents(const char *dir_path); //modified code from project 1
void display_file_details(const char *file_path, const struct dirent *entry);
void display_header(const char *dir_path);
int check_if_command(char *word);  
void call_list();
void call_list_directory(char *path); 
void call_chdir(char *path); 
void call_pwd();
void call_start(char *program, char **arg_list); 
void call_wait(); 
void call_waitfor(pid_t pid); 
void call_kill(pid_t pid); 
void call_run(char *program, char **arg_list); 
void call_array(char *program, int count, char **arg_list);
pid_t str_to_pid(char *str); 

//main
int main() {
    char line[MAX_LINE];
    char *words[MAX_WORDS]; 
    char *new_word; 
     
    while(1) {
        errno = 0;  //reset every time there is new command (so errno doesn't carry over)
        int nwords = 0;
        printf("myshell> ");  
        fflush(stdout); //bug fix may f things up
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
        //get argument list to be passed into execvp()
        char **arg_list = malloc((nwords) * sizeof(char*)); 
        for (int i = 0; i < nwords-1; i++) {
            arg_list[i] = words[i+1]; 
        }
        arg_list[nwords-1] = NULL; //terminate with null
        //for (int i = 0; i < nwords; i++) {
        //    printf("arg_list[%d]: %s\n", i, arg_list[i]); 
        //}
        while(words[num_commands] != 0) {
     
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
                    }
                }
            } 
            if (strcmp("chdir", words[num_commands]) == 0) {
                call_chdir(words[num_commands + 1]); 
            }
            if (strcmp("pwd", words[num_commands]) == 0) {
                call_pwd(); 
            }
            if (strcmp("start", words[num_commands]) == 0) {
                if (words[num_commands + 1] == 0) {
                    printf("error: missing program argument\n"); 
                }
                else { 
                    call_start(words[num_commands +1], arg_list);
                }
    
            }
            if (strcmp("wait", words[num_commands]) == 0) {
                call_wait(); 
            }
            if (strcmp("waitfor", words[num_commands]) == 0) {
                
                if (words[num_commands + 1] == 0) {
                    printf("error: missing pid argument\n"); 
                }
                else {
                    pid_t pid; 
                    pid = str_to_pid(words[num_commands + 1]); 
                    call_waitfor(pid); 
                }
            }
            if (strcmp("kill", words[num_commands]) == 0) {
                pid_t pid; 
                pid = str_to_pid(words[num_commands +  1]); 
                call_kill(pid); 
            }
            if (strcmp("run", words[num_commands]) == 0) {
                
                call_run(words[num_commands +1], arg_list); 
            }
            if (strcmp("array", words[num_commands]) == 0) { 
                //convert count from str to int 
                int count = atoi(words[num_commands+1]);  
                //print error if user doesn't put count or puts 0
                if (count == 0) {
                    printf("Error: Usage: array <count> <command> [arguments]\n"); 
                }
                //modify arg_list so to remove the count
                for (int j = 0; j < nwords - 1; j++) {
                    if (j == nwords -2) {
                        arg_list[j] = NULL; 
                    }
                    arg_list[j] = arg_list[j+1];
                }
                //call array function
                call_array(words[num_commands + 2], count, arg_list); 
            }
            if (strcmp("exit", words[num_commands]) == 0) {
                exit(1);
            }
            if (strcmp("clear", words[num_commands]) == 0) {
                system("clear");
            }
            //display error if unknown command
            if ((strcmp("exit", words[num_commands]) != 0) && (strcmp("array", words[num_commands]) != 0) &&
                    (strcmp("run", words[num_commands]) != 0) && (strcmp("kill", words[num_commands]) != 0) &&
                        (strcmp("waitfor", words[num_commands]) != 0) && (strcmp("wait", words[num_commands]) != 0) &&
                            (strcmp("start", words[num_commands]) != 0) && (strcmp("pwd", words[num_commands]) != 0) &&
                                (strcmp("chdir", words[num_commands]) != 0) && (strcmp("list", words[num_commands]) != 0) &&(num_commands == 0) && (strcmp("clear", words[num_commands]) != 0)) {

                                    printf("myshell: unknown command: %s\n", words[num_commands]); 
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
    char cwd[MAX_LINE]; 
    getcwd(cwd, sizeof(cwd));
    //error handling built into this function from project 1
    display_dir_contents(cwd); 
    printf("\n"); 
}

void call_list_directory(char *path) { //list attributes of given directory
    //error handling built into display_dir_contents from project 1
    display_dir_contents(path); 
    printf("\n"); 
}

void call_chdir(char *path) {
    if (chdir(path) == -1) {
        //errors in the case of chdir failure
        if (errno == ENOENT) {
            printf("myshell: cannot change directory to '%s': No such file or directory\n", path);
        } else if (errno == EACCES) {
            printf("myshell: cannot change directory to '%s': Permission denied\n", path);
        } else {
            printf("myshell: cannot change directory to '%s': %s\n", path, strerror(errno));
        }
    }
    
}

void call_pwd(){
    char cwd[1024]; 
    getcwd(cwd, sizeof(cwd));
    //add error
    printf("%s\n", cwd); 
}


 void call_start(char *program, char**arg_list) {
    pid_t pid; 
    pid = fork(); 
    if (pid < 0) {
        printf("fork error: %s\n", strerror(errno)); 
        return; 
    }
    if (pid == 0) {
        //execlp(program, program, (char *) NULL);
        execvp(program, arg_list); 
        //only runs if execlp fails
        printf("execvp error: %s\n", strerror(errno));
        exit(1);
    }
    else if (pid != 0) {
        printf("myshell: process %d started\n", pid); 
        fflush(stdout); 
        //wait(NULL); 
    }
}


pid_t str_to_pid(char *str) {
    int int_pid; 
    pid_t pid_pid; 
    int_pid = atoi(str);  
    pid_pid = (pid_t)int_pid;
    return pid_pid; 
}

void call_wait() {
    int status;  
    int result = wait(&status);
    pid_t pid = getpid();  
    
    if (result == -1) { //when uncomment^ change to else if 
        if (errno == ECHILD) {
            printf("myshell: no child processes\n");
        } else {
            printf("myshell: wait error: %s", strerror(errno));
        }
        return;
    }
    

    if (WIFEXITED(status)) {  //process exited normally
        printf("myshell: process %d exited normally with status %d\n", pid, WEXITSTATUS(status));
    } 
    else if (WIFSIGNALED(status)) {  //process was terminated by a signal
        int sig = WTERMSIG(status);
        printf("myshell: process %d exited abnormally with signal %d: %s\n", pid, sig, strsignal(sig));
    } 
    else {
        printf("myshell: process %d exited abnormally.\n", pid);
    }
}

void call_waitfor(pid_t pid) { // test when start works 
    int status;
    pid_t result = waitpid(pid, &status, 0); 
    if (result == 0) {
        if (errno == ECHILD) {
            printf("myshell: no such process\n");
        } else {
            perror("myshell: waitpid error");
        }
    } else if (WIFEXITED(status)) {
        printf("myshell: process %d exited normally with status %d\n", pid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("myshell: process %d was terminated by signal %d\n", pid, WTERMSIG(status));
    } else {
        printf("myshell: no such process\n"); 
    }
}

void call_run(char *program, char **arg_list) { //combine start and waitfor functionality 
    pid_t pid; 
    pid = fork(); 
    int error = 0; 
    if (pid < 0) {
        printf("fork error: %s", strerror(errno)); 
    }
    if (pid == 0) {
        //execlp(program, program, (char *) NULL); 
        execvp(program, arg_list); 
        //will only print if execl fails
        printf("execvp() failed to run: %s\n", strerror(errno)); 
        error = 1; 
    }
    if (error == 0) {
        call_waitfor(pid);
    }
}

void call_kill(pid_t pid) {
    if (kill(pid, SIGKILL) == 0) {
        printf("myshell: process %d has been killed\n", pid);
    } else {
        //kill() failed
        if (errno == ESRCH) {
            printf("myshell: no such running process %d\n", pid);
        } else if (errno == EPERM) {
            printf("myshell: permission denied to kill process %d\n", pid);
        } else {
            printf("myshell: error killing process %d: %s\n", pid, strerror(errno));
        }
    }
} 

void call_array(char *program, int count, char **arg_list) {
    if (count <= 0) {
        printf("myshell: invalid count for array command\n");
        return;
    }

    //count args
    int num_args = 0;
    while (arg_list[num_args] != NULL) {
        num_args++;
    }

    for (int i = 0; i < count; i++) {
        //allocate memory for the new arg list
        char **new_args = malloc((num_args + 1) * sizeof(char *));
        if (new_args == NULL) {
            perror("myshell: malloc failed");
            exit(EXIT_FAILURE);
        }

        //replacing @ with the current index
        for (int j = 0; j < num_args; j++) {
            if (strcmp(arg_list[j], "@") == 0) {

                //allocate buffer for the string representation of i
                
                char buf[MAX_WORDS]; //just using an already defined macro
                sprintf(buf, "%d", i);
                new_args[j] = strdup(buf);
                if (new_args[j] == NULL) {
                    perror("myshell: strdup failed");
                    exit(EXIT_FAILURE);
                }
            } else {
                new_args[j] = strdup(arg_list[j]);
                if (new_args[j] == NULL) {
                    perror("myshell: strdup failed");
                    exit(EXIT_FAILURE);
                }
            }
        }

        new_args[num_args] = NULL;

        //run the command
        call_run(program, new_args);

        //free memory 
        for (int j = 0; j < num_args; j++) {
            free(new_args[j]);
        }
        free(new_args);
    }
}




void display_dir_contents(const char *dir_path) {
    DIR *dir;
    struct dirent *entry;
    struct stat status;

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
    char *file_type; //for printing type at end of function

    //get file details
    if (lstat(file_path, &file_status) == -1) {
        fprintf(stderr, "Error: cannot stat %s: %s\n", file_path, strerror(errno));
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



