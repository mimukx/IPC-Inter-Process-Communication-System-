#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 151
int fileDescriptor, pipeDescriptor;

void signal_handler(int signal);

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <pipe_descriptor>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (signal(SIGUSR1, signal_handler) == SIG_ERR)
    {
        perror("Error setting up SIGUSR1 handler");
        exit(EXIT_FAILURE);
    }

    fileDescriptor = open("p2.txt", O_RDONLY);
    if (fileDescriptor < 0)
    {
        perror("Error opening input_p2.txt");
        exit(EXIT_FAILURE);
    }

    pipeDescriptor = atoi(argv[1]);

    printf("P2 process is initialized and ready.\n");
    kill(getppid(), SIGUSR1);

    while (1)
    {
        pause();
    }

    return 0;
}

void signal_handler(int signal)
{
    if (signal != SIGUSR1)
        return;

    char buffer[BUFFER_SIZE];
    char currentChar;
    int bufferIndex = 0;

    memset(buffer, 0, BUFFER_SIZE);

    while (1)
    {
        ssize_t bytesRead = read(fileDescriptor, &currentChar, 1);

        if (bytesRead == 0)
        {

            printf("End of input_p2.txt reached. Terminating process.\n");
            close(fileDescriptor);
            exit(EXIT_SUCCESS);
        }
        else if (bytesRead < 0)
        {
            perror("Error reading from input_p2.txt");
            close(fileDescriptor);
            exit(EXIT_FAILURE);
        }

        buffer[bufferIndex++] = currentChar;

        if (currentChar == '\n' || bufferIndex >= BUFFER_SIZE - 1)
        {
            buffer[bufferIndex] = '\0';
            break;
        }
    }

    if (write(pipeDescriptor, buffer, strlen(buffer)) < 0)
    {
        perror("Error writing to pipe");
    }
}
