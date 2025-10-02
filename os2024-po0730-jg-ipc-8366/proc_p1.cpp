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
        perror("Error setting signal handler for SIGUSR1");
        exit(EXIT_FAILURE);
    }

    fileDescriptor = open("p1.txt", O_RDONLY);
    if (fileDescriptor < 0)
    {
        perror("Error opening input_p1.txt");
        exit(EXIT_FAILURE);
    }

    pipeDescriptor = atoi(argv[1]);

    printf("P1 process initialized and ready.\n");
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

    char lineBuffer[BUFFER_SIZE];
    char currentChar;
    int bytesRead = 0;
    int bufferIndex = 0;

    memset(lineBuffer, 0, BUFFER_SIZE);

    while ((bytesRead = read(fileDescriptor, &currentChar, 1)) > 0)
    {

        lineBuffer[bufferIndex++] = currentChar;

        if (currentChar == '\n' || bufferIndex >= (BUFFER_SIZE - 1))
        {
            lineBuffer[bufferIndex] = '\0';
            break;
        }
    }

    if (bytesRead == 0)
    {

        printf("End of input_p1.txt reached. Terminating process.\n");
        close(fileDescriptor);
        exit(EXIT_SUCCESS);
    }
    else if (bytesRead < 0)
    {
        perror("Error reading from input_p1.txt");
        close(fileDescriptor);
        exit(EXIT_FAILURE);
    }

    if (write(pipeDescriptor, lineBuffer, strlen(lineBuffer)) < 0)
    {
        perror("Error writing to pipe");
    }
}

