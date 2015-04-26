// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"
extern int GPid;
extern int Last_Bg_Pid;
extern int Susp_Bg_Pid;
extern int susp;
extern pJob job_list_head;
extern char* L_Fg_Cmd;


void fill_action(struct sigaction *action, void(*treatment)(int))
{
	sigfillset(&(action->sa_mask));
	action->sa_flags = 0;
	action->sa_handler = treatment;
}

void ctrl_z_handler (int signal)
{
	int dead_job;
	if (GPid != -1)
	{
		dead_job=kill(GPid,SIGTSTP);
		printf("signal SIGTSTP was sent to pid %d\n",GPid);

		if(!dead_job)
		{
			insert_job(&job_list_head, GPid, L_Fg_Cmd, SUSPENDED);
			Susp_Bg_Pid=GPid;
			Last_Bg_Pid=GPid;
			GPid=-1;
			L_Fg_Cmd=NULL;
		}

	}
}

void ctrl_c_handler (int signal)
{
	if (GPid != -1)
	{
		kill(GPid,SIGINT);
		printf("signal SIGINT was sent to pid %d\n",GPid);
	}
}
#if 0
void sigchld_handler (int signal)
{
	int child_pid;
	int ret_val;
	int status;
	child_pid = waitpid(-1, &status, WNOHANG);
	ret_val=remove_job(&job_list_head, child_pid);
	printf("DEBUG: function sigchld_handler, ret_val= %d, child_pid=%d\n",ret_val,child_pid);
	if((ret_val == -1)) //meaning there was no such child process in the job list because it ran in the FG
	{
		GPid=-1;
		L_Fg_Cmd=NULL;
	}
}
#endif
