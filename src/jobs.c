#include "jobs.h"

void insert(job_node *head, job_node *node)
{
    job_node *temp = head->next;
    head->next = node;
    node->next = temp;
}

void jobs(job_node *head)
{
    job_node *curr_node;
    while(curr_node)
    {
        waitpid
    }
}