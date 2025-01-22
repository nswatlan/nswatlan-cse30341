 //nswatlan - CSE 30341 Project 1
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <dirent.h>


#define MAX 1024

//function to get file type 
const char *file_type(mode_t mode) {
    if (S_ISREG(mode)) return "file";
    if (S_ISDIR(mode)) return "dir";
    if (S_ISLNK(mode)) return "link";
    return "unknown";
}

//function to display contents 
void display(const char *path, const struct stat *statbuf, const char *type) {
    if (strcmp(type, "file") == 0) {
        FILE *fp = fopen(path, "r");
        //get contents if it is a file
        if (fp) {
            char buffer[128];
            if (fgets(buffer, sizeof(buffer), fp)) {
                buffer[strcspn(buffer, "\n")] = 0;  // strip newline
                printf("  %s ", buffer);
            }
            fclose(fp);
        }

    } else if (strcmp(type, "dir") == 0) {
        printf(" (directory)");
    } else if (strcmp(type, "link") == 0) {
        printf(" (symlink target)");
    }
}

//function to scan directory contents
void scan_dir(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("Error: Directory not found");
        return;
    }

    struct dirent *entry;
    struct stat statbuf;
    struct passwd *pw;
    //struct group *gr;

    //initialize totals
    int files = 0;
    int dirs = 0;
    int links = 0;

    //get time to be displayed 
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%H:%M:%S on %m/%d/%Y", local);

    printf("Contents of %s on %s\n", path, time_str);
    printf("\nNAME             SIZE    TYPE MODE    OWNER CONTENTS\n");
    printf("--------------------------------------------------------------------------------\n");

    //read directory entries in given directory
    while ((entry = readdir(dir)) != NULL) {
        //skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char full_path[MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (stat(full_path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        //get file type, owner, and permissions
        const char *type = file_type(statbuf.st_mode);
        pw = getpwuid(statbuf.st_uid);
       // gr = getgrgid(statbuf.st_gid);
        
        //print file details
        printf("%-15s %6ld B  %-4s %04o", entry->d_name, statbuf.st_size, type, statbuf.st_mode & 0777);
        if (!pw) {
            printf("unknown");
        }
        else {
            printf("  %-8s",pw->pw_name);
        }
        //print everthing
        display(full_path, &statbuf, type);
        printf("\n");

        // count totals
        if (strcmp(type, "file") == 0) files++;
        if (strcmp(type, "dir") == 0) dirs++;
        if (strcmp(type, "link") == 0) links++;
    }

    closedir(dir);
    printf("--------------------------------------------------------------------------------\n");
    printf("total: %d files, %d directories, and %d symlinks\n", files, dirs, links);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Error: Incorrect number of arguments"); 
        return -1;
    }

    const char *path = argv[1];

    while (1) {
        system("clear");  
        scan_dir(path);
        sleep(3);  // updates every three seconds
    }

    return 0;
}