#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

#include "jobs.h"

job_node *head;

void create_job(pid_t pid, char *name)
{
    job_node *new_job = malloc(sizeof(job_node));
    if (!new_job)
    {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    strcpy(new_job->name, name);
    new_job->pid = pid;
    job_node *temp = head;
    head = new_job;
    new_job->next = temp;
}

void jobs()
{
    job_node *current_node = head;
    job_node *previous_node = NULL;
    int status;
    while (current_node)
    {
        pid_t child_pid = waitpid(current_node->pid, &status, WNOHANG);
        printf("%u, %u\n", status, child_pid);
        if (!WIFEXITED(status)) // child process has terminated
        {
            // delete node
            if (!previous_node)
            {
                job_node *to_delete = current_node;
                current_node = current_node->next;
                free(to_delete);
            }
            else
            {
                previous_node->next = current_node->next;
                free(current_node);
                current_node = previous_node->next;
            }
        }
        else
        {
            // print job
            printf("PID %u | %s\n", current_node->pid, current_node->name);
            previous_node = current_node;
            current_node = previous_node->next;
        }
    }
}