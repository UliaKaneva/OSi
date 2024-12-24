#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main(int argc, char *argv[]) {
    close(STDOUT_FILENO);
    if (argc != 2) {
        write(STDERR_FILENO, "Usage: child1 <filename>\n", 25);
        exit(EXIT_FAILURE);
    }

    int file = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file == -1) {
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LINE_LENGTH];
    ssize_t len;
    while ((len = read(STDIN_FILENO, buffer, MAX_LINE_LENGTH)) > 0) {
        if (strcmp(buffer, "exit") == 0) {
            close(file);
            break;
        }
        reverse_string(buffer);
        write(file, buffer, len - 1);
        write(file, "\n", 1);
    }

    return 0;
}