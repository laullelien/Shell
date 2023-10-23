#include <sys/types.h>

#define JOB_NAME_MAX_LENGTH 20

typedef struct
{
    job_node *next;
    char name[JOB_NAME_MAX_LENGTH];
    pid_t pid;
} job_node;

void insert(job_node *head, job_node *node);

void jobs(job_node *head);

