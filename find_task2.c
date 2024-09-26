#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <string.h>

// File type constants
#define TYPE_REGULAR 0
#define TYPE_DIRECTORY 1
#define TYPE_CHARACTER_DEVICE 2
#define TYPE_BLOCK_DEVICE 3
#define TYPE_FIFO 4
#define TYPE_SOCKET 5
#define TYPE_SYMLINK 6
#define TYPE_UNKNOWN 7

#define BUF_SIZE 1024

// Structure for directory entry
struct directory_entry {
    ino_t          d_ino;    // Inode number
    off_t          d_off;    // Offset to the next dirent
    unsigned short d_reclen; // Length of this record
    char           d_name[]; // Filename (null-terminated)
};

// Function to map file type to integer
int get_file_type(const struct stat* file_stat) {
    if (S_ISREG(file_stat->st_mode)) return TYPE_REGULAR;
    if (S_ISDIR(file_stat->st_mode)) return TYPE_DIRECTORY;
    if (S_ISCHR(file_stat->st_mode)) return TYPE_CHARACTER_DEVICE;
    if (S_ISBLK(file_stat->st_mode)) return TYPE_BLOCK_DEVICE;
    if (S_ISFIFO(file_stat->st_mode)) return TYPE_FIFO;
    if (S_ISSOCK(file_stat->st_mode)) return TYPE_SOCKET;
    if (S_ISLNK(file_stat->st_mode)) return TYPE_SYMLINK;
    return TYPE_UNKNOWN;
}

// Function to get file type as a string
const char* file_type_to_string(int file_type) {
    switch (file_type) {
    case TYPE_REGULAR: return "REG";
    case TYPE_DIRECTORY: return "DIR";
    case TYPE_CHARACTER_DEVICE: return "CHR";
    case TYPE_BLOCK_DEVICE: return "BLK";
    case TYPE_FIFO: return "FIFO";
    case TYPE_SOCKET: return "SOCK";
    case TYPE_SYMLINK: return "SYMLINK";
    default: return "UNKNOWN";
    }
}

// Function to process directory entries
void list(const char* root, off_t min_size, int max_type) {
    int fd;
    char buffer[BUF_SIZE];
    struct directory_entry* d;
    struct stat sb;
    int nread;
    char full_path[1024];
    int pos;

    // Open the directory
    fd = open(root, O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        perror("open");
        return;
    }

    // Read directory entries
    while ((nread = syscall(SYS_getdents, fd, buffer, BUF_SIZE)) > 0) {
        for (pos = 0; pos < nread;) {
            d = (struct directory_entry*)(buffer + pos);

            // Skip "." and ".." directories
            if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
                pos += d->d_reclen;
                continue;
            }

            // Construct the full path of the entry
            snprintf(full_path, sizeof(full_path), "%s/%s", root, d->d_name);

            // Get file statistics using lstat
            if (lstat(full_path, &sb) == -1) {
                perror("lstat");
                pos += d->d_reclen;
                continue;
            }

            // Check file type and size
            int file_type = get_file_type(&sb);
            off_t file_size = sb.st_size;

            // Check if the file matches the criteria
            if (file_size >= min_size && file_type <= max_type) {
                printf("%s %ldKB %s\n", full_path, file_size / 1024, file_type_to_string(file_type));
            }

            // If the entry is a directory, recursively list its contents
            if (S_ISDIR(sb.st_mode)) {
                list(full_path, min_size, max_type);
            }

            pos += d->d_reclen;
        }
    }

    if (nread == -1) {
        perror("syscall(SYS_getdents)");
    }

    // Close the directory
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <root_directory> <size_in_KB> <file_type_integer>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* root = argv[1];
    off_t min_size = atoll(argv[2]) * 1024; //KB to bytes
    int max_type = atoi(argv[3]);

    if (max_type < 0 || max_type > 7) {
        fprintf(stderr, "Invalid file type integer. Must be between 0 and 7.\n");
        exit(EXIT_FAILURE);
    }

    // List all files in the given root directory that meet the criteria
    list(root, min_size, max_type);

    return 0;
}
