/*#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

void files(const char *base_path) {
    char path[1000];
    struct dirent *dp;
    struct stat statbuf;
    DIR *dir = opendir(base_path);

    // Unable to open
    if (!dir) {
        perror("Unable to read");
        return;
    }

    while ((dp = readdir(dir)) != NULL) {
        // Ignore the "." and ".." entries
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;

        // Construct new path
        snprintf(path, sizeof(path), "%s/%s", base_path, dp->d_name);

        // Get stats
        if (stat(path, &statbuf) == -1) {
            perror("stat");
            continue;
        }


        if (S_ISREG(statbuf.st_mode)) {
            printf("%s\n", path);
        }

        // recursively call to files if DIR
        if (S_ISDIR(statbuf.st_mode)) {
            files(path);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <root_directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    files(argv[1]);

    return 0;
}*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <string.h>

#define BUF_SIZE 1024

void list_files(const char* root_directory) {
    int fd;
    char buffer[BUF_SIZE];
    struct linux_dirent {
        long           d_ino;
        off_t          d_off;
        unsigned short d_reclen;
        char           d_name[];
    };
    struct linux_dirent* d;
    struct stat file_stat;
    int nread;
    char path[1024];
    char d_type;

    // Open the directory
    fd = open(root_directory, O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        perror("open");
        return;
    }

    // Read directory entries
    while ((nread = syscall(SYS_getdents, fd, buffer, BUF_SIZE)) > 0) {
        for (int bpos = 0; bpos < nread;) {
            d = (struct linux_dirent*)(buffer + bpos);
            d_type = *(buffer + bpos + d->d_reclen - 1);

            // Skip "." and ".."
            if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
                bpos += d->d_reclen;
                continue;
            }

            // Construct the full path
            snprintf(path, sizeof(path), "%s/%s", root_directory, d->d_name);

            // Get file information
            if (stat(path, &file_stat) == -1) {
                perror("stat");
                continue;
            }

            // Check if it's a regular file
            if (S_ISREG(file_stat.st_mode)) {
                printf("%s\n", path);
            }
            else if (S_ISDIR(file_stat.st_mode)) {
                // Recursively list files in subdirectories
                list_files(path);
            }

            bpos += d->d_reclen;
        }
    }

    if (nread == -1) {
        perror("syscall(SYS_getdents)");
    }

    // Close the directory
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <root_directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // List all files in the given root directory
    list_files(argv[1]);

    return 0;
}
