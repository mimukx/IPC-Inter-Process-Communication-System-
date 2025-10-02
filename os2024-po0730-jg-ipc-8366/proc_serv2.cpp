#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    if (port <= 0) {
        fprintf(stderr, "Invalid port number.\n");
        exit(EXIT_FAILURE);
    }

    //printf("Server initialized. PID=%d, Port=%d\n", getpid(), port);

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    //int reuse = 1;
    //setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr.s_addr);
    server_address.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    int file_fd = open("serv2.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (file_fd < 0) {
        perror("Error opening file");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    kill(getppid(), SIGUSR1);

    //printf("Server ready to receive data.\n");

    char buffer[256];
    //struct sockaddr_in client_address;
    //socklen_t client_address_len = sizeof(client_address);

    for (int i = 0; i < 10; i++) {
        ssize_t received = recv(socket_fd, buffer, 256, 0);
        if (received < 0) {
            perror("Error receiving data");
            close(file_fd);
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        if (write(file_fd, buffer, received) < 0 || write(file_fd, "\n", 1) < 0) {
            perror("Error writing to file");
            close(file_fd);
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        printf("Data logged: %s\n", buffer);
    }

    //printf("Server shutting down.\n");
    kill(getppid(), SIGUSR1);

    close(file_fd);
    close(socket_fd);
    exit(EXIT_SUCCESS);
}
