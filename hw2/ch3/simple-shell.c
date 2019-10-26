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
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

int parse_cmd(char *cmd, char **args, char *ifile, char *ofile, int *split_pos) {
	char *token;
	int i = 0;
	char redirect;
	
	token = strtok(cmd, " "); /* take the first token */

	while (token) {
		if (!strcmp(token, "<") || !strcmp(token, ">")) { /* redirect */
			redirect = token[0];
		}
		else {
			switch (redirect) {
				case '<':
					strcpy(ifile, token);
					break;

				case '>':
					strcpy(ofile, token);
					break;
				
				default:
					args[i] = (char*) malloc(sizeof(char) * (strlen(token)));
					strcpy(args[i], token);

					if (!strcmp(token, "|")) *split_pos = i;

					++i;
			}
		}

		token = strtok(NULL, " "); /* take the next token */
	}

	if (!strcmp(args[i - 1], "&")) 
	{
		args[i - 1] = NULL; /* clear '$' */
	
		return 0; /* wait flag is set to 0 (concurrent) */
	}
	
	return 1; /* wait flag is set to 1 */
}

void print_args(char **args) {
	int i = 0;

	while (args[i]) {
		printf("%d\n", i);
		printf("args[%d] is %s\n", i, args[i]);
		++i;
	}
}

int exe(char **args, int wait_flag, char *ifile, char *ofile, int split_pos) {
	pid_t pid;
	off_t fd;
	int exe_err;

	pid = fork();

	if (pid < 0) { /* error occurred */
		fprintf(stderr, "Fork Failed.\n");
		return 1;
	}
	else if (pid == 0) { /* child process */
		if (strlen(ifile)) {
			fd = open(ifile, O_RDWR);
			
			if (fd < 0) {
				printf("Error occurs when opening %s.\n", ifile);
				exit(0);
			}
			
			dup2(fd, STDIN_FILENO);
		}
		else if (strlen(ofile))	
		{
			fd = open(ofile, O_RDWR|O_CREAT, S_IRWXU);

			if (fd < 0) {
				printf("Error occurs when opening %s\n", ofile);
				exit(0);
			}
			
			dup2(fd, STDOUT_FILENO);
		}
		
		exe_err = execvp(args[0], args);

		if (exe_err < 0) printf("The command is not executable.\n");

		// close(fd);
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
	char cmd_history[MAX_LINE]; /* history buffer */
	int wait_flag;
	int split_pos;

	char ifile[20];
	char ofile[20];

	for (int i = 0; i < MAX_LINE/2 + 1; ++i) args[i] = NULL;
	cmd_history[0] = '\0';
	ifile[0] = '\0';
	ofile[0] = '\0';
	
	while (should_run) {   
		printf("osh>");
		fflush(stdout);

		fgets(cmd, MAX_LINE, stdin);
		cmd[strlen(cmd) - 1] = '\0'; /* delete line-feed */

		/**
		 * After reading user input, the steps are:
		 * (1) fork a child process
		 * (2) the child process will invoke execvp()
		 * (3) if command included &, parent will invoke wait()
		 */

		if (!strcmp(cmd, "!!")) {
			if (strlen(cmd_history) == 0) 
			{
				printf("-----------------------\n");
				printf("No commands in history.\n");
				printf("-----------------------\n");
			}
			else {
				printf("-------------------------------------------------\n");
				printf("Command '%s' in the history will be executed.\n", cmd_history);
				printf("-------------------------------------------------\n");

				strcpy(cmd, cmd_history);
			}
		}	
		else strcpy(cmd_history, cmd);

		wait_flag = parse_cmd(cmd, args, ifile, ofile, &split_pos);

		printf("Input file: %s; output file: %s\n", ifile, ofile);

		printf("Split position: %d\n", split_pos);

		exe(args, wait_flag, ifile, ofile, split_pos);


		// clear values
		for (int i = 0; i < MAX_LINE/2 + 1; ++i) 
		{	
			free(args[i]);
			args[i] = NULL;
		}
		ifile[0] = '\0';
		ofile[0] = '\0';
	}
    
	return 0;
}
