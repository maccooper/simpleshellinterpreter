/*
 *	ssi.c
 *
 *	CSC360 Assignment #1
 *	Name:		Mackenzie Cooper
 *	Student #:	V00892515
 *	Spring 2021
 *	
 *	Note: I took this course last semester prior to dropping it.
 *	As a result, some code has been recycled from my PMan submission last term.
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/mman.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include "csc360_list.h"

#define BUFFER_SIZE 1000
#define USER_BUFFER_MAX_SIZE 100
#define HOST_BUFFER_MAX_SIZE 100
#define CWD_BUFFER_MAX_SIZE 80
#define PROMPT_SIZE 300

char **string_tokenize(char *s)
{
	//Input:        a string with spaces
	//Output:       2d array with substrings from s
	//helper function for p_stat
	char *ptr = strtok(s, " ");
	char **args = (char **)malloc(BUFFER_SIZE * sizeof(char *));
	int i = 0;
	while (ptr != NULL) {
		args[i] = ptr;
		ptr = strtok(NULL, " ");
		i++;
	}
	return args;
}

void fetch_prompt(char s_prompt[PROMPT_SIZE])
{	
	//helper function for fetching username, hostname, current working dir
	char *buf_user = getlogin();

	char buf_host[HOST_BUFFER_MAX_SIZE];
	gethostname(buf_host, sizeof(buf_host));

	char buf_cwd[CWD_BUFFER_MAX_SIZE];
	getcwd(buf_cwd, sizeof(buf_cwd));	

	s_prompt[0] = 0;
	strncat(s_prompt, buf_user, USER_BUFFER_MAX_SIZE);
	strncat(s_prompt, "@", 2);
	strncat(s_prompt, buf_host, HOST_BUFFER_MAX_SIZE);
	strncat(s_prompt, buf_cwd, CWD_BUFFER_MAX_SIZE);
	strncat(s_prompt, ">", 2);
}

void update_process()
{
	int status;
	int pid = waitpid(-1, &status, WNOHANG|WUNTRACED|WCONTINUED);
	if(pid > 0) {
		if(WIFCONTINUED(status)) {
			Node *n = find_node(pid);
			if(n) {
				n->run_state = 1;
			} else  if (WIFSTOPPED(status)) {
				Node *n = find_node(pid);
				if(n) {
					n->run_state = 0;
				}
			} else if (WIFEXITED(status)) {
				remove_node(pid);
				printf("%d Finished\n", pid);
			} else if (WIFSIGNALED(status)) {
				remove_node(pid);
				printf("%d Terminated\n", pid);
			}

		}
	}
}

void bg_entry(char **argv, int arglength)
{
	//Input:        char ** array containing string args for process to run in background
	//Output:       None
	//runs a process in the background while parent continues to read input. Adds pid to linkedlist datastructure
	pid_t pid;
	pid = fork();
	update_process();
	if (pid == 0) {
		//in child process
		if (execvp(argv[1], &argv[1]) < 0) {
			perror("Error on execvp");
			exit(-1);
		}
	} else if (pid > 0) {
		//parent Process
		//store into our LL
		if (errno != ENOENT) {
			usleep(1000);
			Node *n = new_node(pid, argv[1], 1);
			head = add_front(head, n);
		}
	} else {
		//error with fork (pid < 0)
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
}

void bg_list()
{
	//writes information from linked list to console (pid, process_name)

	Node *curr = head;
	int process_counter = 0;
	while (curr != NULL) {
		process_counter++;
		printf("%i:\t%s\n", curr->pid, curr->process_name);
		curr = curr->next;
	}
	printf("Total background jobs:\t%i\n", process_counter);
}

void change_dir(char **args)
{
	if (args[1] == NULL || strcmp(args[1],"~") == 0) {
		chdir(getenv("HOME"));
	} else if (strcmp(args[1], "..") == 0){
		chdir("..");
	} else { 
		if(chdir(args[1]) != 0) {
			perror("chdir() failed");
		}
	}
}

void ls_command(char **argv, int arglength)
{
	char *argument_list[arglength + 1];
	int i;
	for(i = 0; i < arglength; ++i) {
		argument_list[i] = argv[i];
	}
	argument_list[arglength] = NULL;
	int status;
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		status = execvp("ls", argument_list);
		if(status == -1) {
			printf("Terminated Incorrectly\n");
		}
	} else if (pid > 0) {
		wait(&status);
	} else {
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
}

int process_exists(int pid)
{
	char dir_buffer[BUFFER_SIZE];
	sprintf(dir_buffer, "/proc/%d", pid);
	DIR *dir = opendir(dir_buffer);
	if(!dir) {
		if (ENOENT == errno) {
			printf("error: process %d does not exist.\n", pid);
		} else {
			printf("error: process %d is unaccessible.\n", pid);
		}
		return 0;
	}
	return 1;
}

void bg_stop(int pid)
{
	//sends STOP signal to target <pid>
	if (!process_exists(pid)) 
		return;
	if (kill(pid, SIGSTOP)) {
		printf("error: failed to stop process %d\n", pid);
	}
}

void bg_start(int pid)
{
	//sends CONT signal to target <pid>
	if (!process_exists(pid)) 
		return;
	if (kill(pid, SIGCONT)) {
		printf("error: failed to start process %d\n", pid);
	}
}

void bg_kill(int pid) {
	//sends TERM signal to target <pid>
	if (!process_exists(pid)) 
		return;
	Node *target = find_node(pid);
	if (target && target->run_state == 0) {
		bg_start(pid);
	}
	usleep(1000);
	if (kill(pid, SIGTERM)) {
		printf("error: failed to killed process %d\n", pid);
	}
	
}

void dispatch_command(char **args, int length)
{
	//manages function calls based on console command
	if (!(strcasecmp(args[0], "bglist"))) {
	//	printf("bg list command here\n");
		bg_list();
	}
	else if (!(strcasecmp(args[0], "bg"))) {
	//	printf("bg entry command\n");
	bg_entry(args, length);
	}
	else if (!(strcasecmp(args[0], "ls"))) {
		ls_command(args, length);
	}
	else if (!(strcasecmp(args[0], "cd"))) {
		if(length > 2){
			printf("Too many arguments for cd\n");
		} else {
			change_dir(args);	
		}
	}
	else if (!(strcasecmp(args[0], "exit"))) {
		printf("Exiting the program\n");
		exit(0);
	} else if (!(strcasecmp(args[0], "bgstop"))) {
		bg_stop(atoi(args[1]));
	} else if (!(strcasecmp(args[0], "bgstart"))) {
		bg_start(atoi(args[1]));
	} else if (!(strcasecmp(args[0], "bgkill"))) {
		bg_kill(atoi(args[1]));
	} else {
		printf("Command not recognized\n");
	}
}

int main()
{
	int i, j;
	char *token = " ";
	//char *prompt = ": >";
	char prompt[PROMPT_SIZE];
	fetch_prompt(prompt);
	const int max_args = 100;
	//	char *a = fetch_prompt();
	//printf("%s\n", a);
	while (1) {

		i = 0;
		char *input = NULL;
		char *iterToken;
		char *args[max_args];
		update_process();
		input = readline(prompt);
		if (!strcmp(input, "")) {
			continue;
		}
		iterToken = strtok(input, token);
		for (j = 0; j < max_args; j++) {
			if (iterToken)
				i++;	//Tracks number of arguments
			args[j] = iterToken;	//grabs tokens 
			iterToken = strtok(NULL, token);
		}
		dispatch_command(args, i);
		//printf("\n");
		update_process();
		fetch_prompt(prompt);
	}
	free_list(head);
}
