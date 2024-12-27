#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024
#define SHARED_MEM_SIZE (MAX_LINE_LENGTH + 1)
#define SHARED_MEM_NAME1 "/shared_memory1"
#define SHARED_MEM_NAME2 "/shared_memory2"
#define SEM_NAME1 "/sem1"
#define SEM_NAME2 "/sem2"

void invert_string(char *str) {
    unsigned long len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        write(STDOUT_FILENO, "Usage: <name_program> <filename>\n", 33);
        return EXIT_FAILURE;
    }

    int shm_fd;
    char *shared_mem;
    sem_t *sem;
    if (strcmp(argv[0], "child1") == 0) {
        shm_fd = shm_open(SHARED_MEM_NAME1, O_RDWR, 0);
        sem = sem_open(SEM_NAME1, 0);
    } else {
        shm_fd = shm_open(SHARED_MEM_NAME2, O_RDWR, 0);
        sem = sem_open(SEM_NAME2, 0);
    }
    if (shm_fd == -1 || sem == SEM_FAILED) {
        write(STDOUT_FILENO, "error: open resources\n", 22);
        return EXIT_FAILURE;
    }
    shared_mem = mmap(NULL, SHARED_MEM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        write(STDOUT_FILENO, "error mmap\n", 11);
        return EXIT_FAILURE;
    }

    int file = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file == -1) {
        write(STDOUT_FILENO, "error open file\n", 16);
        return EXIT_FAILURE;
    }

    while (1) {
        sem_wait(sem);
        if (strcmp(shared_mem, "") == 0) {
            break;
        }

        char buffer[MAX_LINE_LENGTH];
        strcpy(buffer, shared_mem);
        invert_string(buffer);

        write(file, buffer, strlen(buffer));
        write(file, "\n", 1);
    }

    munmap(shared_mem, SHARED_MEM_SIZE);
    close(shm_fd);
    close(file);
    sem_close(sem);

    return EXIT_SUCCESS;
}