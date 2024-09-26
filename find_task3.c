#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>



// Function to process directory entries and write results to pipe
void list_files(const char* dir_path, int fd) {
    DIR* dir;
    struct dirent* entry;
    struct stat sb;
    char full_path[1024];
    char buffer[1024];
    int bytes_written;

    if ((dir = opendir(dir_path)) == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        if (lstat(full_path, &sb) == -1) {
            perror("lstat");
            continue;
        }

        // Resolve symbolic link if it's a link
        if (S_ISLNK(sb.st_mode)) {
            char link_target[1024];
            ssize_t len = readlink(full_path, link_target, sizeof(link_target) - 1);
            if (len != -1) {
                link_target[len] = '\0';
                printf("%s -> %s (SYMLINK)\n", full_path, link_target);
                continue;
            }
            else {
                perror("readlink");
                continue;
            }
        }

        // Write file details to pipe
        snprintf(buffer, sizeof(buffer), "%s\n", full_path);
        bytes_written = write(fd, buffer, strlen(buffer));
        if (bytes_written == -1) {
            perror("write");
        }
    }

    closedir(dir);
}

// fork for subdirectory
void subdir_check(const char* root) {
    DIR* dir;
    struct dirent* entry;
    char subpath[1024];
    int fd[2];
    pid_t pid;
    int status;
    struct stat sb;

    if ((dir = opendir(root)) == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(subpath, sizeof(subpath), "%s/%s", root, entry->d_name);

        // If it's a directory, create a child process to handle it
        if (entry->d_type == DT_DIR) {
            if (pipe(fd) == -1) {
                perror("pipe");
                continue;
            }

            pid = fork();
            if (pid == 0) {
                // Child process
                close(fd[0]);
                list_files(subpath, fd[1]);
                close(fd[1]);
                exit(EXIT_SUCCESS);
            }
            else if (pid > 0) {
                // Parent process
                close(fd[1]);
                char buffer[1024];
                ssize_t bytes_read;

                // Read from pipe and write to stdout
                while ((bytes_read = read(fd[0], buffer, sizeof(buffer) - 1)) > 0) {
                    buffer[bytes_read] = '\0';
                    printf("%s", buffer);
                }

                close(fd[0]);
                waitpid(pid, &status, 0);
            }
            else {
                perror("fork");
                continue;
            }
        }
        else {
            // Process regular files in the root directory
            if (lstat(subpath, &sb) == -1) {
                perror("lstat");
                continue;
            }

            printf("%s\n", subpath);
        }
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <root>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* root = argv[1];

    // Process each subdirectory of the root directory
    subdir_check(root);

    return 0;
}
