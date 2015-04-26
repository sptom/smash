/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#include "jobs.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20
extern int GPid;// PID (global)
extern int Last_Bg_Pid;
extern int Susp_Bg_Pid;
extern char* L_Fg_Cmd;
extern int susp; //is the process suspended: 0- no, 1- yes
extern pJob job_list_head;
char lineSize[MAX_LINE_SIZE]; 
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE]; 	   
    pJob* job_list_head_ptr;
    struct sigaction ctrl_z_act;
    struct sigaction ctrl_c_act;
    //struct sigaction sigchld_act;

	fill_action(&ctrl_z_act, &ctrl_z_handler);
	fill_action(&ctrl_c_act, &ctrl_c_handler);
	//fill_action(&sigchld_act, &sigchld_handler);

    sigaction(SIGTSTP, &ctrl_z_act,NULL);
    sigaction(SIGINT, &ctrl_c_act,NULL);
    //sigaction(SIGCHLD,&sigchld_act,NULL);
    int dead_pid;

	//signal declaretions
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	 /* add your code here */
	
	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here
	/* add your code here */

	/************************************/

	/************************************/
    // Init globals
	GPid = -1;
	Last_Bg_Pid = -1;
	Susp_Bg_Pid = -1;
	susp = 0;
	job_list_head = NULL;
	job_list_head_ptr = &job_list_head;


#if 0
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL) 
			exit (-1); 
	L_Fg_Cmd[0] = '\0';
#endif
    	while (1)
    	{
    	printf("smash > ");

    		while((dead_pid=waitpid(-1,NULL,WNOHANG))>0)// This loop is in charge of handling zombies
    		{
    			remove_job(&job_list_head,dead_pid);
    		}
    		if((dead_pid==-1) && (errno=EINTR))
    		{
    			//refresh_list(job_list_head_ptr);
    		}

		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';
					// perform a complicated Command
		if(!ExeComp(lineSize)) continue; 
					// background command	
	 	if(!BgCmd(lineSize, job_list_head_ptr)) continue;
					// built in commands
		ExeCmd(&job_list_head, lineSize, cmdString);
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
		fflush(stdout);
		fflush(stdin);
	}
    return 0;
}

