#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "jobs.h"
#define NOT_SUSPENDED 0
#define SUSPENDED 1

void fill_action(struct sigaction *action, void(*treatment)(int));
void ctrl_z_handler (int signal);
void ctrl_c_handler (int signal);
//void sigchld_handler (int signal);

#endif

