//Norah Swatland - CSE 30341 Project 1
//dirwatch.c

#define _GNU_SOURCE

#include <sys/stat.h>
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

//protoypes (functions defined below main)
void display_dir_contents(const char *dir_path);
void display_header(const char *dir_path);
void display_file_details(const char *file_path, const struct dirent *entry);

int main(int argc, char *argv[]) {
    //checking for path as argv[1]
    if (argc != 2) {
        printf("Usage: ./dirwatch <pathname>\n");
        return -1;
    }

    const char *dir_path = argv[1];
    
    //check if directory exists 
    DIR *test = opendir(dir_path);
    if (!test) {
        printf("Error: Cannot access directory %s: %s\n", dir_path, strerror(errno));
        return -1;
    }
    closedir(test);

    //runs until user indicates end with Ctrl^C
    while (1) {
        system("clear");
        display_dir_contents(dir_path);
        sleep(3); //updates time and contents every three seconds 
    }

    return 0;
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
    display_header(dir_path);
    
    
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
        if (S_ISREG(status.st_mode)) file_count++;
        if (S_ISDIR(status.st_mode)) dir_count++;
        if (S_ISLNK(status.st_mode)) symlink_count++;
    }

    //check for error with readdir
    if (errno != 0) {
        printf("Error: Cannot read directory entries - %s\n", strerror(errno));
    }

    //display counts of file types
    printf("--------------------------------------------------------------------------------\n");
    printf("Total: %d files, %d directories, and %d symlinks\n", 
           file_count, dir_count, symlink_count);

    if (closedir(dir) == -1) {
        printf("Error: Cannot close directory - %s\n", strerror(errno));
    }
}

void display_header(const char *dir_path) {
    time_t current;
    char formatted[BUFSIZ];
    if (time(&current) == -1) {
        printf("Error: cannot get current time: %s\n", strerror(errno));
        return;
    }
    if (strftime(formatted, sizeof(formatted), "%H:%M:%S on %m/%d/%Y", localtime(&current)) == 0) {
        printf("Error: cannot format time\n");
        return;
    }
    printf("Contents of %s on %s\n", dir_path, formatted);

    printf("--------------------------------------------------------------------------------\n");
    printf("NAME            SIZE       TYPE   MODE   OWNER     CONTENTS\n");
    printf("--------------------------------------------------------------------------------\n");

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

    //get file contents based on entry type 
    if (S_ISREG(file_status.st_mode)) {
        fp = open(file_path, O_RDONLY);
        if (fp == -1) {
            fprintf(stderr, "Error: Cannot open %s: %s\n", file_path, strerror(errno));
        } else {
            int bytes_read = read(fp, buffer, sizeof(buffer) - 1);
            if (bytes_read == -1) {
                fprintf(stderr, "Eror: Cannot read %s: %s\n", file_path, strerror(errno));
            } else if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                char *newline_position = strchr(buffer, '\n');
                if (newline_position) *newline_position = '\0';
                if (strncpy(file_contents, buffer, sizeof(file_contents) - 1) == NULL) {
                    fprintf(stderr, "Error: Cannot copy contents for %s\n", file_path);
                }
                file_contents[sizeof(file_contents) - 1] = '\0';
            }
            if (close(fp) == -1) {
                fprintf(stderr, "Error: cannot close %s: %s\n", file_path, strerror(errno));
            }
        }
    }
    else if (S_ISDIR(file_status.st_mode)) {
        strncpy(file_contents, "(directory)", sizeof(file_contents) - 1);
        file_contents[sizeof(file_contents) - 1] = '\0';
    }
    else if (S_ISLNK(file_status.st_mode)) {
        char dest[BUFSIZ] = ""; //link destination
        ssize_t link_length = readlink(file_path, dest, sizeof(dest) - 1);
        if (link_length == -1) {
            fprintf(stderr, "Error: Cannot read symlink %s: %s\n", file_path, strerror(errno));
            strncpy(file_contents, "(invalid link)", sizeof(file_contents) - 1);
        } else {
            dest[link_length] = '\0';
            size_t prefix_length = strlen("symbolic link to ");
            size_t max_length = sizeof(file_contents) - prefix_length - 1;
            size_t dest_length = strlen(dest);
            if (dest_length > max_length) {
                dest_length = max_length;
            }
            if (snprintf(file_contents, sizeof(file_contents), "symbolic link to %.*s",
                        (int)dest_length, dest) < 0) {
                fprintf(stderr, "Error: Cannot format symlink %s\n", file_path);
                return;
            }
        }
        file_contents[sizeof(file_contents) - 1] = '\0';
    }

    //replace bad characters
    for (int i = 0; file_contents[i] != '\0'; i++) {
        if (!isprint(file_contents[i])) {
            file_contents[i] = '*';
        }
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
    printf("%-15s %-7lldB   %-6s %04o   %-8s  %-s\n", entry->d_name, (long long)file_status.st_size, file_type, file_status.st_mode & 0777, owner_user, file_contents);

}
