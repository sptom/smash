#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include "jobs.h"
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define EXT_COMMAND_SIZE 5
typedef enum { FALSE , TRUE } bool;
int ExeComp(char* lineSize);
int BgCmd(char* lineSize, pJob* job_list_head_pointer);
int ExeCmd(pJob* head, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString);
int GPid;// PID (global)
int Last_Bg_Pid;
int Susp_Bg_Pid;
char* L_Fg_Cmd;
int susp; //is the process suspended: 0- no, 1- yes
#endif

