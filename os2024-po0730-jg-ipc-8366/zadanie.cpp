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

// Descriptors for processes and resources
int server1Process, server2Process, processD, processP1, processP2, processPr, processT, processS;

void terminateProcesses(int signal)
{
    sleep(5);

    kill(processP1, SIGKILL);
    kill(processP2, SIGKILL);
    kill(processPr, SIGKILL);
    kill(processT, SIGKILL);
    kill(processS, SIGKILL);
    kill(processD, SIGKILL);
    kill(server1Process, SIGKILL);
    kill(server2Process, SIGKILL);
}

void notifyTaskContinue(int signal)
{
    printf("Task continues.\n");
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        perror("Invalid arguments! Usage: <program> <server1Port> <server2Port>");
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR2, terminateProcesses);
    signal(SIGUSR1, notifyTaskContinue);

    int pipe1[2], pipe2[2];
    pipe(pipe1);
    pipe(pipe2);

    int semaphore1 = semget(IPC_PRIVATE, 2, 0666 | IPC_CREAT);
    int semaphore2 = semget(IPC_PRIVATE, 2, 0666 | IPC_CREAT);
    if (semaphore1 == -1 || semaphore2 == -1)
    {
        perror("Failed to create semaphores.");
        exit(EXIT_FAILURE);
    }

    semctl(semaphore1, 0, SETVAL, 0);
    semctl(semaphore2, 0, SETVAL, 0);
    semctl(semaphore1, 1, SETVAL, 0);
    semctl(semaphore2, 1, SETVAL, 0);

    int server1Port = atoi(argv[1]);
    int server2Port = atoi(argv[2]);

    int sharedMemory1 = shmget(IPC_PRIVATE, 256, 0666 | IPC_CREAT);
    int sharedMemory2 = shmget(IPC_PRIVATE, 256, 0666 | IPC_CREAT);

    int pipe1Read = dup(pipe1[0]);
    int pipe2Read = dup(pipe2[0]);
    int pipe1Write = dup(pipe1[1]);
    int pipe2Write = dup(pipe2[1]);

    char pipe1ReadStr[10], pipe2ReadStr[10], pipe1WriteStr[10], pipe2WriteStr[10];
    char semaphore1Str[10], semaphore2Str[10];
    char sharedMemory1Str[10], sharedMemory2Str[10];
    char server1PortStr[10], server2PortStr[10];

    sprintf(pipe1ReadStr, "%d", pipe1Read);
    sprintf(pipe2ReadStr, "%d", pipe2Read);
    sprintf(pipe1WriteStr, "%d", pipe1Write);
    sprintf(pipe2WriteStr, "%d", pipe2Write);
    sprintf(semaphore1Str, "%d", semaphore1);
    sprintf(semaphore2Str, "%d", semaphore2);
    sprintf(sharedMemory1Str, "%d", sharedMemory1);
    sprintf(sharedMemory2Str, "%d", sharedMemory2);
    sprintf(server1PortStr, "%d", server1Port);
    sprintf(server2PortStr, "%d", server2Port);

    printf("Initializing Server 1...\n");
    server1Process = fork();
    if (server1Process == 0)
    {
        execl("proc_serv1", "proc_serv1", server1PortStr, server2PortStr, NULL);
        perror("Failed to start Server 1");
        exit(EXIT_FAILURE);
    }
    pause();

    printf("Initializing Server 2...\n");
    server2Process = fork();
    if (server2Process == 0)
    {
        execl("proc_serv2", "proc_serv2", server2PortStr, NULL);
        perror("Failed to start Server 2");
        exit(EXIT_FAILURE);
    }
    pause();

    printf("Initializing Process D...\n");
    processD = fork();
    if (processD == 0)
    {
        execl("proc_d", "proc_d", sharedMemory2Str, semaphore2Str, server1PortStr, NULL);
        perror("Failed to start Process D");
        exit(EXIT_FAILURE);
    }
    pause();

    printf("Initializing Process P1...\n");
    processP1 = fork();
    if (processP1 == 0)
    {
        execl("proc_p1", "proc_p1", pipe1WriteStr, NULL);
        perror("Failed to start Process P1");
        exit(EXIT_FAILURE);
    }
    pause();

    printf("Initializing Process P2...\n");
    processP2 = fork();
    if (processP2 == 0)
    {
        execl("proc_p2", "proc_p2", pipe1WriteStr, NULL);
        perror("Failed to start Process P2");
        exit(EXIT_FAILURE);
    }
    pause();

    char string_p1R[10];
    char string_p2R[10];
    char string_P1[10];
    char string_P2[10];
    char string_p1W[10];
    char string_p2W[10];

    printf("Initializing Process Pr...\n");

    
    sprintf(string_P1, "%d", processP1);
    sprintf(string_P2, "%d", processP2);
    sprintf(string_p1R, "%d", pipe1Read);
    sprintf(string_p2W, "%d", pipe2Write);

    
    printf("Arguments for proc_pr: P1=%s, P2=%s, p1R=%s, p2W=%s\n",
           string_P1, string_P2, string_p1R, string_p2W);

    processPr = fork();
    if (processPr == 0)
    {
        execl("proc_pr", "proc_pr", string_P1, string_P2, string_p1R, string_p2W, (char *)NULL);
        perror("Failed to start Process Pr");
        exit(EXIT_FAILURE);
    }
    pause();

    printf("Initializing Process T...\n");
    processT = fork();
    if (processT == 0)
    {
        execl("proc_t", "proc_t", pipe2ReadStr, sharedMemory1Str, semaphore1Str, NULL);
        perror("Failed to start Process T");
        exit(EXIT_FAILURE);
    }
    pause();

    printf("Initializing Process S...\n");
    processS = fork();
    if (processS == 0)
    {
        execl("proc_s", "proc_s", sharedMemory1Str, semaphore1Str, sharedMemory2Str, semaphore2Str, NULL);
        perror("Failed to start Process S");
        exit(EXIT_FAILURE);
    }
    
	pause();
	pause();
    return 0;
}