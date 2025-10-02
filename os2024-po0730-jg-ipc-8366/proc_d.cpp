#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>

int semaphore_id;

static int release_semaphore();
static int acquire_semaphore();

int main(int argc, char *argv[]) {
    kill(getppid(), SIGUSR1);

    struct sockaddr_in server_address;
    struct hostent *server;

    semaphore_id = atoi(argv[2]);

    printf("Process D is creating a socket.\n");
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("Process D socket creation failed!");
        exit(EXIT_FAILURE);
    }

    char buffer[256] = {'\0'};

    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        perror("Host not found");
        exit(EXIT_FAILURE);
    }

    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr.s_addr);
    server_address.sin_port = htons(atoi(argv[3]));

    printf("Process D is establishing a connection.\n");
    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Process D failed to connect to the server!");
        exit(EXIT_FAILURE);
    }

    printf("Process D is sending data.\n");

    char *shared_memory = (char *)shmat(atoi(argv[1]), NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("Shared memory attachment failed!");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (!release_semaphore()) {
            exit(EXIT_FAILURE);
        }

        strcpy(buffer, shared_memory);
        write(socket_fd, buffer, strlen(buffer) + 1);
        memset(buffer, '\0', sizeof(buffer));

        if (!acquire_semaphore()) {
            exit(EXIT_FAILURE);
        }

        sleep(1);
    }

    return 0;
}

static int release_semaphore() {
    struct sembuf sem_operation;
    sem_operation.sem_op = 1;
    sem_operation.sem_num = 0;
    sem_operation.sem_flg = SEM_UNDO;

    if (semop(semaphore_id, &sem_operation, 1) == -1) {
        perror("Semaphore release failed!");
        return 0;
    }
    return 1;
}

static int acquire_semaphore() {
    struct sembuf sem_operation;
    sem_operation.sem_op = -1;
    sem_operation.sem_num = 1;
    sem_operation.sem_flg = SEM_UNDO;

    if (semop(semaphore_id, &sem_operation, 1) == -1) {
        perror("Semaphore acquisition failed!");
        return 0;
    }
    return 1;
}
