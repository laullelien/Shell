#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <assert.h>
#include <stdio.h>

#include "jobs.h"
#include "readcmd.h"

#define MAX_PROCESS_NUMBER 64

pid_t pid_list[MAX_PROCESS_NUMBER];

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
        pid_t pid = fork();

        if (pid == -1) // fail to fork
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (!pid) // son pid = 0
        {
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