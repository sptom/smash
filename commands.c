//		commands.c
//********************************************
#include "commands.h"
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(pJob* head, char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	static char last_dir[MAX_LINE_SIZE];
	char* delimiters = " \t\n";  
	int i = 0;
	/*========used in kill command=======*/
	int signal_number, Job_number, signal_sent;
	int found_job_pid;
	char found_job_name[MAX_LINE_SIZE];
	/*===================================*/
	/*========used in fg command=========*/
	int child_done_id, fg_runner_pid, fg_req_JID;
	/*===================================*/
	/*========used in bg command=========*/
	int bg_runner_pid;
	/*===================================*/

	bool illegal_cmd = FALSE; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
   	L_Fg_Cmd = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters);
	}
/*************************************************/
// Built in Commands
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{
		getcwd(pwd,sizeof(pwd));
		if(!strcmp(args[1], "-"))
		{
			if(last_dir[0]!=0)
			{
				chdir(last_dir);
			}
			else
			{
				perror("smash error: > no previous path exists\n");
			}
		}
		else
		{

			if(!chdir(args[1]))
			{
				strcpy(last_dir,pwd);
			}
			else
			{
				printf("smash error: > \"%s\" -path not found\n",args[1]);
				illegal_cmd = TRUE;
			}
		}
	} 
	
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		if(getcwd(pwd,sizeof(pwd))!= NULL)
		{
			fprintf(stdout,"%s\n",pwd);
		}
		else
		{
			perror("smash error: > pwd error\n");
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "mkdir"))
	{

		if(mkdir(args[1], 0755))
			{
				if(EEXIST == errno)
				{
					printf("smash error: > \"%s\" -directory already exists\n",args[1]);
				}
				else
				{
					printf("smash error: > \"%s\" -cannot create directory\n",args[1]);
				}
			}
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs")) 
	{
		print_jobs(*head);
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		if(args[1][0] != '-')
		{
			printf("smash error: > \"%s\"",cmdString);
			return -1;
		}
		signal_number=atoi(args[1]+1);
		Job_number=atoi(args[2]);

		//search requested job
		found_job_pid=find_job_PID(*head,Job_number);

		if(found_job_pid==-1)
		{
			printf("smash error: > kill %d - job does not exist\n",Job_number);
			return -1;
		}
		else
		{

			switch (signal_number)
			{
				case SIGTSTP:
							signal_sent=kill(found_job_pid,SIGTSTP);

							printf("signal SIGTSTP was sent to pid %d\n",found_job_pid);
							if(signal_sent!=0)
							{
								printf("smash error: > kill %d - cannot send signal\n",Job_number);
							}
							strcpy(found_job_name,find_job_name(*head,found_job_pid));
							remove_job(head,found_job_pid);
							insert_job(head, found_job_pid, found_job_name, SUSPENDED);
							break;
				case SIGINT:
							signal_sent=kill(found_job_pid,SIGINT);

							printf("signal SIGINT was sent to pid %d\n",found_job_pid);
							if(signal_sent!=0)
							{
								printf("smash error: > kill %d - cannot send signal\n",Job_number);
							}
							remove_job(head,found_job_pid);
							break;
				case SIGTERM:
							signal_sent=kill(found_job_pid,SIGTERM);

							printf("signal SIGTERM was sent to pid %d\n",found_job_pid);
							if(signal_sent!=0)
							{
								printf("smash error: > kill %d - cannot send signal\n",Job_number);
							}
							remove_job(head,found_job_pid);
							break;
				default:
							signal_sent=kill(found_job_pid,signal_number);
							if(signal_sent!=0)
							{
								printf("smash error: > kill %d - cannot send signal\n",Job_number);
							}
							printf("signal %d was sent to pid %d\n",signal_number,found_job_pid);
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		printf("smash pid is %d\n",getpid());
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		if(args[1]==NULL)
		{
			Last_Bg_Pid=get_last_job_pid(*head);
			fg_runner_pid=Last_Bg_Pid;
		}
		else
		{
			fg_req_JID=atoi(args[1]);
			fg_runner_pid=find_job_PID(*head,fg_req_JID);
		}
		if (fg_runner_pid== -1)// No jobs in the Bg
		{
				perror("smash error: requested process not found\n");
		}
		else if(is_suspended(*head,fg_runner_pid)== SUSPENDED)
		{
			kill(fg_runner_pid,SIGCONT);
			printf("signal SIGCONT was sent to pid %d\n",fg_runner_pid);
			unsuspend(*head,fg_runner_pid);
			Susp_Bg_Pid=get_last_suspended_job_pid(*head);
		}
		GPid=fg_runner_pid;
		L_Fg_Cmd = find_job_name(*head,fg_runner_pid);
		do
		{
			child_done_id = wait(NULL);
			if((child_done_id==-1)&&(errno==EINTR)) //child is gone due to inerrupt
			{
				break;
			}
		}
		while((child_done_id != fg_runner_pid) && (fg_runner_pid != Susp_Bg_Pid));
		remove_job(head,fg_runner_pid);
		GPid= -1;

	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{

		if(args[1]==NULL)
			{
				bg_runner_pid=get_last_suspended_job_pid(*head);
			}
		else
			{
				fg_req_JID=atoi(args[1]);
				bg_runner_pid=find_job_PID(*head, fg_req_JID);
				if(is_suspended(*head,bg_runner_pid)==NOT_SUSPENDED)
				{
					perror("smash error: > bg called on un-suspended job\n");
					return -1;
				}
			}
		strcpy(found_job_name,find_job_name(*head,bg_runner_pid));
		printf("%s",found_job_name);
		kill(bg_runner_pid,SIGCONT);
		printf("signal SIGCONT was sent to pid %d\n",bg_runner_pid);
		unsuspend(*head,bg_runner_pid);
		Susp_Bg_Pid=get_last_suspended_job_pid(*head);
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		if(args[1]==NULL)
		{
			destroy_list(head);
	   		exit(0);
		}
		else if (!strcmp(args[1], "kill"))
		{
			if (kill_jobs(*head) != -1) {
				destroy_list(head);
				exit(0);
			}
			else {
				exit (-1);
			}
		}
		else
			perror("smash error: > quit called with illegal argument\n");
			return -1;
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString);
	 	return 0;
	}
	if (illegal_cmd == TRUE)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
	int child_done_id;
	/* Initializing the args_ext array last cell to be NULL for the execv */
	L_Fg_Cmd=cmdString;
    switch(pID = fork())
	{
    		case -1: 
					// Add your code here (error)
					perror("error: process creation failed in ExeExternal\n");
					return;
					break;
        	case 0 :
					// Child process
					setpgrp();
					fflush(stdout);
					fflush(stdin);
					//execv(args_ext[0],args_ext);
					execvp(args[0],args);
					/* if we got here, that means execv has faild */
					perror("error: process execution failed\n");
					exit(-1);
					break;
			default:
                	GPid=pID;
                	do
					{
						child_done_id = waitpid(pID,NULL,0);
						if((child_done_id==-1)&&(errno==EINTR))
						{
							break;
						}
					}
					while((child_done_id != pID) && (pID != Susp_Bg_Pid));
                	GPid=-1;
                	return;
	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	int pID;
	int child_done_id;
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args_comp[EXT_COMMAND_SIZE];
	/* Initializing the args_comp array last cell to be NULL for the execv */
	args_comp[EXT_COMMAND_SIZE-1]=NULL;

	char csh_call[9]="/bin/csh";
	char f_flag[3]="-f";
	char c_flag[3]="-c";

	L_Fg_Cmd=lineSize;

    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
    	fflush(stdout);
    	switch(pID=fork())
    	{
    	case -1:
    		perror("error: process creation failed in ExeComp\n");
    		return -1;
    		break;
    	case 0:
    		// Child process
    		setpgrp();
    		ExtCmd[0]='\0';
    		lineSize[strlen(lineSize)-1]='\0';
    		strcat(ExtCmd,lineSize);
    		fflush(stdout);
    		args_comp[0]=csh_call;
    		args_comp[1]=f_flag;
    		args_comp[2]=c_flag;
    		args_comp[3]=ExtCmd;
       		fflush(stdout);
       		fflush(stdin);
       		execv(args_comp[0],args_comp);
       		/* if we got here, that means execv has faild */
       		perror("error: process execution failed\n");
       		exit(-1);
       		break;
    	default:
    		/* Meaning we are the proud parent */
    		GPid=pID;
    		do
    		{
    			child_done_id = wait(NULL);
    		} while((child_done_id != pID) && (pID != Susp_Bg_Pid));
    		return 0;
    	}
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, pJob* job_list_head_pointer)
{
	int pID;
	char* command;
	char *args[MAX_ARG];
	char* delimiters = " \t\n";
	int i;

	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
    	command = strtok(lineSize, delimiters);
		if (command == NULL)
			return -1;
		args[0] = command;
		for (i=1; i<MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters);
		}
		fflush(stdout);
		switch(pID=fork())
		{
			case -1:
				perror("error: process creation failed in BgCmd\n");
				return -1;
				break;
			case 0:
				// Child process
				setpgrp();
				fflush(stdout);
				execvp(args[0],args);
				/* if we got here, that means execv has faild */
				perror("error: process execution failed\n");
				exit(-1);
				break;
			default:
				command=strtok(lineSize,delimiters);
				Last_Bg_Pid=pID;
				/* we are the proud father */
				insert_job(job_list_head_pointer,pID,command,NOT_SUSPENDED);
				return 0;
		}
	}
	return -1;
}

