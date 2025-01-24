//
//norah
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
#include <signal.h>

//fix comments 

//global variables for terminal size
int terminal_rows = 0;
int terminal_cols = 0;


//prototypes
void display_dir_contents(const char *dir_path);
void display_header(const char *dir_path, int content_width);
void display_file_details(const char *file_path, const struct dirent *entry, int content_width);
void get_terminal_size(); 
void handle_resize(int sig); 

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./dirwatch <pathname>\n");
        return -1;
    }
    //set path to argv[1]
    const char *dir_path = argv[1];

    //test that directory can open
    DIR *test = opendir(dir_path);
    if (!test) {
        printf("Error: Cannot access directory %s: %s\n", dir_path, strerror(errno));
        return -1;
    }
    closedir(test);

    //signal handler for resizing 
    signal(SIGWINCH, handle_resize);
    get_terminal_size(); //initial terminal size

    while (1) {
        system("clear");
        display_dir_contents(dir_path);
        sleep(3); //updates every 3 seconds
    }

    return 0;
}

void display_dir_contents(const char *dir_path) {
    DIR *dir;
    struct dirent *entry;
    struct stat status;
    int file_count = 0, dir_count = 0, symlink_count = 0;

    dir = opendir(dir_path);
    if (!dir) {
        printf("Error: Cannot open directory %s: %s\n", dir_path, strerror(errno));
        return;
    }

    //for resizing, get content width
    int content_width = terminal_cols - 50; 
    if (content_width < 10) content_width = 10;

    //display header
    display_header(dir_path, content_width);

    int displayed_lines = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (displayed_lines >= terminal_rows - 3) { //account for header and summary lines
            printf("(truncated)\n");
            break;
        }

        char entry_path[BUFSIZ];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", dir_path, entry->d_name);

        if (lstat(entry_path, &status) == -1) {
            fprintf(stderr, "Error: Cannot stat %s: %s\n", entry_path, strerror(errno));
            continue;
        }

        display_file_details(entry_path, entry, content_width);
        displayed_lines++;

        if (S_ISREG(status.st_mode)) file_count++;
        if (S_ISDIR(status.st_mode)) dir_count++;
        if (S_ISLNK(status.st_mode)) symlink_count++;
    }

    printf("--------------------------------------------------------------------------------\n");
    printf("Total: %d files, %d directories, and %d symlinks\n",
           file_count, dir_count, symlink_count);

    if (closedir(dir) == -1) {
        fprintf(stderr, "Error: Cannot close directory - %s\n", strerror(errno));
    }
}

void display_header(const char *dir_path, int content_width) {
    time_t current;
    char formatted[BUFSIZ];
    if (time(&current) == -1) {
        printf("Error: Cannot get current time: %s\n", strerror(errno));
        return;
    }
    if (strftime(formatted, sizeof(formatted), "%H:%M:%S on %m/%d/%Y", localtime(&current)) == 0) {
        printf("Error: Cannot format time\n");
        return;
    }

    printf("Contents of %s on %s\n", dir_path, formatted);
    printf("--------------------------------------------------------------------------------\n");
    printf("NAME            SIZE       TYPE   MODE   OWNER     CONTENTS\n");
    printf("--------------------------------------------------------------------------------\n");
}

void display_file_details(const char *file_path, const struct dirent *entry, int content_width) {
    struct stat file_status;
    char owner_user[BUFSIZ] = "unknown";
    struct passwd *user_info;
    char file_contents[BUFSIZ] = "";
    char buffer[BUFSIZ] = "";
    int fp;
    const char *file_type; //for setting types to display at end of function

    if (lstat(file_path, &file_status) == -1) {
        fprintf(stderr, "Error: Cannot stat %s: %s\n", file_path, strerror(errno));
        return;
    }

    user_info = getpwuid(file_status.st_uid);
    if (user_info) {
        strncpy(owner_user, user_info->pw_name, sizeof(owner_user) - 1);
        owner_user[sizeof(owner_user) - 1] = '\0';
    }

    if (S_ISREG(file_status.st_mode)) {
        fp = open(file_path, O_RDONLY);
        if (fp == -1) {
            fprintf(stderr, "Error: Cannot open %s: %s\n", file_path, strerror(errno));
        } else {
            int bytes_read = read(fp, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                strncpy(file_contents, buffer, content_width - 1);
                file_contents[content_width - 1] = '\0';
            }
            close(fp);
        }
    } else if (S_ISDIR(file_status.st_mode)) {
        strncpy(file_contents, "(directory)", content_width - 1);
        file_contents[content_width - 1] = '\0';
    } else if (S_ISLNK(file_status.st_mode)) {
        char dest[BUFSIZ] = "";
        ssize_t link_length = readlink(file_path, dest, sizeof(dest) - 1);
        if (link_length != -1) {
            dest[link_length] = '\0';
            snprintf(file_contents, content_width, "symbolic link to %s", dest);
        } else {
            strncpy(file_contents, "(invalid link)", content_width - 1);
        }
        file_contents[content_width - 1] = '\0';
    }

    for (int i = 0; file_contents[i] != '\0'; i++) {
        if (!isprint(file_contents[i])) file_contents[i] = '*';
    }

    if (S_ISREG(file_status.st_mode)) {
        file_type = "file";
    } else if (S_ISDIR(file_status.st_mode)) {
        file_type = "dir";
    } else if (S_ISLNK(file_status.st_mode)) {
        file_type = "link";
    } else {
        file_type = "unknown";
    }

    printf("%-15s %-7lldB   %-6s %04o   %-8s  %-.*s\n", entry->d_name, (long long)file_status.st_size, file_type, file_status.st_mode & 0777, owner_user, content_width, file_contents);
}

//function to get terminal size
void get_terminal_size() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        perror("ioctl");
        terminal_rows = 24; // default
        terminal_cols = 80;
    } else {
        terminal_rows = w.ws_row;
        terminal_cols = w.ws_col;
    }
}

//signal handler for window resizing
void handle_resize(int sig) {
    get_terminal_size();
}