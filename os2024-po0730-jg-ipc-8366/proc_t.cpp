#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <errno.h>

int semaphore;

static int semaphore_release();
static int semaphore_acquire();

int main(int argc, char const *argv[]) {
    char input_line[256] = {0};
    char char_buffer;
    int index = 0;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <pipe_fd> <shm_id> <sem_id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    semaphore = atoi(argv[3]);
    char *shared_memory = (char *)shmat(atoi(argv[2]), NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("Failed to attach shared memory");
        exit(EXIT_FAILURE);
    }

    kill(getppid(), SIGUSR1);

    int pipe_fd = atoi(argv[1]);
    while (1) {
        index = 0;
        while (read(pipe_fd, &char_buffer, 1) > 0) {
            if (char_buffer == '\n') break;
            input_line[index++] = char_buffer;
            input_line[index] = '\0';
        }

        if (!semaphore_release()) exit(EXIT_FAILURE);

        strcpy(shared_memory, input_line);

        if (!semaphore_acquire()) exit(EXIT_FAILURE);
    }

    return 0;
}

static int semaphore_release() {
    struct sembuf semaphore_operation;
    semaphore_operation.sem_op = 1;
    semaphore_operation.sem_num = 1;
    semaphore_operation.sem_flg = SEM_UNDO;

    if (semop(semaphore, &semaphore_operation, 1) == -1) {
        perror("Error releasing semaphore");
        return 0;
    }
    return 1;
}

static int semaphore_acquire() {
    struct sembuf semaphore_operation;
    semaphore_operation.sem_op = -1;
    semaphore_operation.sem_num = 0;
    semaphore_operation.sem_flg = SEM_UNDO;

    if (semop(semaphore, &semaphore_operation, 1) == -1) {
        perror("Error acquiring semaphore");
        return 0;
    }
    return 1;
}
