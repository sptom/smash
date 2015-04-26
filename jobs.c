/*
 * jobs.c
 *
 *  Created on: 17 ���� 2015
 *      Author: Dehu
 */
#include <sys/wait.h>
#include "jobs.h"

struct job {
    char job_name[MAX_JOB_NAME];
    time_t start_time;
    int jobID;
    int PID;
    pJob pNext;
    int suspended;
};

int insert_job(pJob *head, int PID, char *name, int suspension) {
    static int last_job_id = 0;

    pJob temp_job, curr_job;
    temp_job = (pJob) malloc(sizeof(Job));
    if (temp_job == NULL) {
        exit(-1);
    }
    temp_job->PID = PID;
    strcpy(temp_job->job_name, name);
    temp_job->pNext = NULL;
    temp_job->start_time = time(0);
    temp_job->suspended = suspension;
    temp_job->jobID = (last_job_id + 1);
    last_job_id++;


    if (*head == NULL) {
        *head = temp_job;
        return 0;
    }
    else {
        curr_job = *head;
        while (curr_job->pNext != NULL) {
            curr_job = curr_job->pNext;
        }
        curr_job->pNext = temp_job;
        return 0;
    }
    return 0;
}

int find_job_PID(pJob head, int target_jobID) {
    pJob curr;
    if (head == NULL) {
        printf("smash error: > \"find\" called on empty job list\n");
        return -2;
    }
    curr = head;
    do {
        if (curr->jobID == target_jobID) {
            return curr->PID;
        }
        curr = curr->pNext;
    } while (curr != NULL);

    printf("smash error: > requested job does not exist in list\n");
    return -1;
}

char *find_job_name(pJob head, int target_PID) {
    pJob curr;
    if (head == NULL) {
        printf("smash error: > \"find\" called on empty job list\n");
        return NULL;
    }
    curr = head;
    do {
        if (curr->PID == target_PID) {
            return curr->job_name;
        }
        curr = curr->pNext;
    } while (curr != NULL);

    printf("smash error: > requested job does not exist in list\n");
    return NULL;
}


pJob aux_find_job(pJob head, int PID) {
    pJob curr;
    if (head == NULL) {
        return NULL;
    }
    curr = head;
    do {
        if (curr->PID == PID) {
            return curr;
        }
        curr = curr->pNext;
    } while (curr != NULL);
    return NULL;
}

int remove_job(pJob *head, int target_PID) {
    pJob curr;
    pJob target;
    if (*head == NULL) {
        return -1;
    }
    curr = *head;
    if (curr->PID == target_PID) {
        *head = curr->pNext;
        free(curr);
        return 0;
    }
    while (curr->pNext != NULL) {
        if (curr->pNext->PID == target_PID) {
            target = curr->pNext;
            curr->pNext = target->pNext;
            free(target);
            return 0;
        }
        curr = curr->pNext;
    }
    return -1;
}

void print_jobs(pJob head) {
    pJob current;
    time_t now = time(0);
    double elapsed;
    if (head == NULL) {
        return;
    }
    current = head;
    do {
        elapsed = difftime(now, current->start_time);
        if (current->suspended == SUSPENDED) {
            printf("[%d] %s : %d %.0lf secs (Stopped)\n", current->jobID, current->job_name, current->PID, elapsed);
        }
        else {
            printf("[%d] %s : %d %.0lf secs\n", current->jobID, current->job_name, current->PID, elapsed);
        }
        current = current->pNext;
    } while (current != NULL);

    return;
}


int get_last_job_pid(pJob head) {
    pJob current;
    if (head == NULL) {
        return -1;
    }
    current = head;
    while (current->pNext != NULL) {
        current = current->pNext;
    }
    return current->PID;
}

int get_last_suspended_job_pid(pJob head) {
    int last_suspended = -1;
    pJob current;
    if (head == NULL) {
        return -1;
    }
    current = head;
    do {
        if (current->suspended == SUSPENDED) {
            last_suspended = current->PID;
        }
        current = current->pNext;
    }
    while (current != NULL);
    return last_suspended;
}

int is_suspended(pJob head, int PID) {
    pJob target;
    target = aux_find_job(head, PID);
    if (target == NULL) {
        return -1;
    }
    return target->suspended;
}

int unsuspend(pJob head, int target_PID) {
    pJob target;
    target = aux_find_job(head, target_PID);
    if (target == NULL) {
        return -1;
    }
    if (target->suspended == NOT_SUSPENDED) {
        return -2; //function was called on an unsuspended job
    }
    if (target->suspended == SUSPENDED) {
        target->suspended = NOT_SUSPENDED;
        return 0;
    }
    return -3; // suspension has junk value (something is very wrong)
}

#if 0
int status;
pid_t return_pid = waitpid(process_id, &status, WNOHANG); /* WNOHANG def'd in wait.h */
if (return_pid == -1) {
    /* error */
} else if (return_pid == 0) {
    /* child is still running */
} else if (return_pid == process_id) {
    /* child is finished. exit status in   status */
}

#endif

int kill_jobs(pJob job_list_head) {
    pJob current = job_list_head;
    int status, return_pid;
    while (current != NULL) {
        printf("[%d] %s - Sending SIGTERM...", current->jobID, current->job_name);
        kill(current->PID, SIGTERM);
        sleep(5);
        return_pid = waitpid(current->PID, &status, WNOHANG);
        if (return_pid == -1)
        {
            /*Error has occurred*/
            destroy_list(job_list_head);
            return -1;
        } else if (return_pid == 0)
        {
            /*process is still running, SIGTERM didn't work*/
            printf(" (5 sec passed) Sending SIGKILL...");
            kill(current->PID, SIGKILL);
        }

        printf(" Done.\n");
        current = current->pNext;
    }
    return 0;
}

void destroy_list(pJob *job_list_head_ptr) {
    pJob to_be_destroyed;
    pJob current = *job_list_head_ptr;

    while (current != NULL) {
        to_be_destroyed = current;
        current = current->pNext;
        free(to_be_destroyed);
    }
}
