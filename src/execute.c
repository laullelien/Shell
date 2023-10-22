#include <sys/types.h>
#include <unistd.h>

#include "readcmd.h"

void execute(struct cmdline *l)
{
    char **cmd = l->seq[0];
    pid_t pid = fork();
    if (!pid)
    {
        execvp(cmd[0], cmd);
    }
}