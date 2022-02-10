/*
 *	ssi.c
 *
 *	CSC360 Assignment #1
 *	Name:		Mackenzie Cooper
 *	Student #:	V00892515
 *	Spring 2021
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
		printf("\n");
		free(input);
		fetch_prompt(prompt);
	}
	free_list(head);
}
