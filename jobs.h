/*
 * jobs.h
 *
 *  Created on: 17 ���� 2015
 *      Author: Dehu
 */

#ifndef JOBS_H_
#define JOBS_H_
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define MAX_JOB_NAME 50
#define NOT_SUSPENDED 0
#define SUSPENDED 1
typedef struct job Job, * pJob;
pJob job_list_head;
/* jobs list interface functions */
int insert_job(pJob* head,int PID,char *name,int suspension);
int find_job_PID(pJob head, int target_PID);
int remove_job(pJob* head, int target_PID);
void print_jobs(pJob head);
int is_suspended(pJob head,int PID);
int get_last_job_pid(pJob head);
char* find_job_name(pJob head, int target_PID);
int get_last_suspended_job_pid(pJob head);
int unsuspend(pJob head,int target_PID);
void destroy_list(pJob * job_list_head_ptr);
int kill_jobs(pJob job_list_head);
#endif /* JOBS_H_ */
