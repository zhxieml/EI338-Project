/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

int parse_cmd(char *cmd, char **args) {
	char *token;
	int i = 0;
	
	token = strtok(cmd, " ");

	while (token) {
		printf("%s\n", token);

		args[i] = (char*) malloc(sizeof(char) * (strlen(token)));
		
		strcpy(args[i], token);

		++i;

		token = strtok(NULL, " ");
	}

	printf("Last arg is %s\n", args[i - 1]);

	if (!strcmp(args[i - 1], "&")) 
	{
		args[i - 1] = NULL;
	
		return 0;
	}
	
	return 1;
}

void print_args(char **args) {
	int i = 0;

	while (args[i]) {
		printf("%d\n", i);
		printf("args[%d] is %s\n", i, args[i]);
		++i;
	}
}

int exe(char **args, int wait_flag) {
	pid_t pid;

	pid = fork();

	if (pid < 0) { /* error occurred */
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	else if (pid == 0) { /* child process */
		execvp(args[0], args);
	}
	else { /* parent process */
		if (wait_flag) wait(NULL);
	}
}

int main(void)
{
	char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
	int should_run = 1;
	char cmd[MAX_LINE];
	int wait_flag;

	for (int i = 0; i < MAX_LINE/2 + 1; ++i) args[i] = NULL;
	
	while (should_run) {   
		printf("osh>");
		fflush(stdout);

		fgets(cmd, MAX_LINE, stdin);
		cmd[strlen(cmd) - 1] = '\0';

		wait_flag = parse_cmd(cmd, args);

		print_args(args);

		exe(args, wait_flag);
		
		/**
		 * After reading user input, the steps are:
		 * (1) fork a child process
		 * (2) the child process will invoke execvp()
		 * (3) if command included &, parent will invoke wait()
		 */

		for (int i = 0; i < MAX_LINE/2 + 1; ++i) args[i] = NULL;
	}
    
	return 0;
}
