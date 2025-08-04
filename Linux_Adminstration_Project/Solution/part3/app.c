#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

int pipefd1[2], pipefd2[2];
char buffer[1024];
int pid;

void worker1(void);
void worker2(void);
void parent(void);
void int_handle(int signum);

int main()
{
    // Create pipes for communication
    if (pipe(pipefd1) == -1 || pipe(pipefd2) == -1)
    {
        perror("pipe failed");
        return 1;
    }

    pid = fork();
    if (pid < 0)
    {
        perror("Fork failed");
        return 1;
    }

    if (pid == 0)
    {
        worker1();
    }
    else
    {
        // Parent process
        printf("Parent process PID: %d\n", getpid());
        close(pipefd1[1]); // Close write end of first pipe

        pid = fork();
        if (pid < 0)
        {
            perror("Fork failed");
            return 1;
        }

        if (pid == 0)
        {
            worker2();
        }
        else
        {
            parent();
        }
    }

    while(1);
}

void worker1(void)
{
    // Worker1 process
    printf("        Worker1 process started with PID: %d\n", getpid());
    close(pipefd1[0]);               // Close read end
    dup2(pipefd1[1], STDOUT_FILENO); // Redirect stdout to pipe
    close(pipefd1[1]);               // Close write end after duplicating

    char *args[] = {"mpstat", NULL};
    execv("/usr/bin/mpstat", args);
    perror("execv failed for mpstat");
    exit(1);
}

void worker2(void)
{
    // Worker2 process of the parent
    printf("        Worker2 process started with PID: %d\n", getpid());
    close(pipefd2[0]);               // Close read end
    dup2(pipefd2[1], STDOUT_FILENO); // Redirect stdout to pipe
    close(pipefd2[1]);

    char *args[] = {"ps", NULL};
    execv("/usr/bin/ps", args);
    perror("execv failed for ps");
    exit(1);
}

void parent(void)
{

    signal(SIGINT, int_handle); // Ignore SIGINT in parent

    // Parent process
    close(pipefd2[1]); // Close write end of second pipe

    // Read and format output from Worker1
    printf("\n=== OUTPUT FROM WORKER1 (mpstat) ===\n");
    while (read(pipefd1[0], buffer, sizeof(buffer) - 1) > 0)
    {
        buffer[strcspn(buffer, "\0")] = '\0'; // Ensure null termination
        printf("%s", buffer);
    }
    close(pipefd1[0]);

    printf("\n=== OUTPUT FROM WORKER2 (ps) ===\n");
    while (read(pipefd2[0], buffer, sizeof(buffer) - 1) > 0)
    {
        buffer[strcspn(buffer, "\0")] = '\0'; // Ensure null termination
        printf("%s", buffer);
    }
    close(pipefd2[0]);

    printf("\n=== ALL PROCESSES COMPLETED ===\n");
}

void int_handle(int signum)
{
    wait(NULL); // Wait for first child
    wait(NULL); // Wait for second child
    exit(0);
}