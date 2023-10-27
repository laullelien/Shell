#include "time.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void init_sigaction()
{
    struct sigaction sa;
    sa.sa_handler = time_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(0);
    }
}