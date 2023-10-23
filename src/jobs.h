#include <sys/types.h>

#define JOB_NAME_MAX_LENGTH 20

typedef struct job_node
{
    struct job_node *next;
    char name[JOB_NAME_MAX_LENGTH];
    pid_t pid;
} job_node;

void create_job(pid_t pid, char *name);

void jobs(void);

