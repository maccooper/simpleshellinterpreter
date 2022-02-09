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

const int buffer_size = 1000;
	
char **string_tokenize(char *s)
{
	//Input:        a string with spaces
	//Output:       2d array with substrings from s
	//helper function for p_stat
	char *ptr = strtok(s, " ");
	char **args = (char **)malloc(buffer_size * sizeof(char *));
	int i = 0;
	while (ptr != NULL) {
		args[i] = ptr;
		ptr = strtok(NULL, " ");
		i++;
	}
	return args;
}


char *fetch_prompt()
{
	char *buf_log;
	buf_log = (char *)malloc(10*sizeof(char));
	buf_log = getlogin();
	return buf_log;
}

int main()
{
	int i, j;
	char *token = " ";
	char *prompt = ": >";
	const int max_args = 100;
	printf("\n %s \n", fetch_prompt());
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
	}
	free_list(head);
}
