#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "jobs.h"
#include "readcmd.h"

#define MAX_PROCESS_NUMBER 64

pid_t pid_list[MAX_PROCESS_NUMBER];
int X;
struct rlimit limit;

void execute(struct cmdline *l)
{
    int pid_nb = 0;
    int fd[2];

    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; l->seq[i] != 0; i++)
    {
        char **cmd = l->seq[i];
        if (!strcmp(cmd[0], "jobs"))
        {
            jobs();
            continue;
        }
        else if(!strcmp(cmd[0], "ulimit")) {
            X = atoi(cmd[1]);
            limit.rlim_cur = X;
            limit.rlim_max = X + 5;
            continue;
        }
        pid_t pid = fork();

        if (pid == -1) // fail to fork
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (!pid) // son pid = 0
        {
            printf("%i\n", X);
            if(X != 0) {
                printf("%i\n", X);
                if(setrlimit(RLIMIT_CPU, &limit) != 0) {
                    printf("not succes\n");
                    perror("Setrlimit failed");
                }
                else{
                    printf("succes\n");
                }
            }
            // first process
            // We only redirect if there is a pipe
            if (i == 0 && l->seq[i + 1] != NULL)
            {
                close(fd[0]);
                dup2(fd[1], 1);
            }
            if (i == 1)
            {
                close(fd[1]);
                dup2(fd[0], 0);
            }
            // output redirection
            if (l->seq[i + 1] == NULL)
            {
                if (l->out)
                {
                    int newfd = open(l->out, O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR);
                    if (newfd == -1)
                    {
                        perror("open file");
                        exit(EXIT_FAILURE);
                    }
                    dup2(newfd, 1);
                }
            }
            // input redirection
            if (l->in && i == 0)
            {
                int newfd = open(l->in, O_RDONLY);
                if (newfd == -1)
                {
                    perror("open file");
                    exit(EXIT_FAILURE);
                }
                dup2(newfd, 0);
            }
            execvp(cmd[0], cmd);
            // if command doesn't exist
            fprintf(stderr, "command not found\n");
            exit(0);
        }
        else // daddy pid != 0
        {
            if (l->bg)
            {
                create_job(pid, cmd[0]);
            }
            pid_list[pid_nb] = pid;
        }

        pid_nb++;
    }

    close(fd[0]);
    close(fd[1]);

    // We wait for all child processes to finish
    if (!l->bg)
    {
        while (pid_nb)
        {
            pid_nb--;
            waitpid(pid_list[pid_nb], NULL, 0);
        }
    }
}