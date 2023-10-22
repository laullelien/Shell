#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <assert.h>

#include "readcmd.h"

#define MAX_PROCESS_NUMBER 100

pid_t pid_list[MAX_PROCESS_NUMBER];

void execute(struct cmdline *l)
{
    int pid_nb = 0;
    for (int i = 0; l->seq[i] != 0; i++)
    {
        char **cmd = l->seq[i];
        pid_t pid = fork();
        if (!pid)
        {
            execvp(cmd[0], cmd);
        }
        else
        {
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