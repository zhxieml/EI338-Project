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
#define READ_END		0
#define WRITE_END		1

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

int get_former_args(char **args, int split_pos, char **former_args) {	
	if (split_pos <= 0) return 1;

	for (int i = 0; i < split_pos; ++i) {
		former_args[i] = args[i];
	}

	former_args[split_pos] = NULL;

	return 0;
}

int get_latter_args(char **args, int split_pos, char **latter_args) {
	if (split_pos <= 0) return 1;

	int i = 0;

	while (args[i + split_pos + 1])
	{
		latter_args[i] = args[i + split_pos + 1];
		++i;
	}

	if (i == 0) return 1;

	latter_args[i] = NULL;

	return 0;
}

int exe(char **args, int wait_flag, char *ifile, char *ofile, int split_pos) {
	pid_t pid;
	off_t fd;
	int pipe_fd[2];
	int exe_err;

	pid = fork(); /* create child process */

	if (pid < 0) { /* error occurred */
		fprintf(stderr, "Fork Failed.\n");
		return 1;
	}
	else if (pid == 0) { /* child process */
		if (strlen(ifile)) { /* redirect to input file */
			fd = open(ifile, O_RDWR);
			
			if (fd < 0) {
				printf("Error occurs when opening %s.\n", ifile);
				exit(0);
			}
			
			dup2(fd, STDIN_FILENO);
		}
		else if (strlen(ofile)) /* redirect to output file */
		{
			fd = open(ofile, O_RDWR|O_CREAT, S_IRWXU);

			if (fd < 0) {
				printf("Error occurs when opening %s\n", ofile);
				exit(0);
			}
			
			dup2(fd, STDOUT_FILENO);
		}

		if (split_pos == 0) { /* no pipe */
			/* execute the command in the child process */
			exe_err = execvp(args[0], args); 

			if (exe_err < 0) printf("The command is not executable.\n");
		}
		else {
			char *former_args[MAX_LINE/2 + 1];
			char *latter_args[MAX_LINE/2 + 1];
			pid_t pipe_pid;

			if (get_former_args(args, split_pos, former_args) != 0 || get_latter_args(args, split_pos, latter_args) != 0) {
				printf("Error occurs when separating the command.\n");
				return 1;
			}

			/* create the pipe */
			if (pipe(pipe_fd) == -1) {
				fprintf(stderr, "Pipe failed");
				return 1;
			}

			/* fork a child process */
			pipe_pid = fork();

			if (pipe_pid < 0) { /* error occurred */
				fprintf(stderr, "Fork Failed");
				return 1;
			}
			else if(pipe_pid == 0) { /* child process */
				/* close the unused end of the pipe */
				close(pipe_fd[READ_END]);

				dup2(pipe_fd[WRITE_END], STDOUT_FILENO);

				/* execute the command in the child process */
				exe_err = execvp(former_args[0], former_args); 

				if (exe_err < 0) printf("The former command is not executable.\n");

				/* close the write end of the pipe */
				close(pipe_fd[WRITE_END]);
			}
			else { /* parent process */
				waitpid(pid, NULL, 0);

				/* close the unused end of the pipe */
				close(pipe_fd[WRITE_END]);

				dup2(pipe_fd[READ_END], STDIN_FILENO);

				print_args(latter_args);

				exe_err = execvp(latter_args[0], latter_args);

				if (exe_err < 0) printf("The latter command is not executable.\n");

				close(pipe_fd[READ_END]);
			}
		}
	}
	else { /* parent process */
		if (wait_flag) waitpid(pid, NULL, 0); /* the parent process will wait if the wait flag is set */
	}
}

int main(void)
{
	/* some initializations */
	char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
	int should_run = 1;
	char cmd[MAX_LINE];
	char cmd_history[MAX_LINE]; /* history buffer */
	int wait_flag; /* flag indicating whether the proecesses are concurrently executed */
	int split_pos; /* position where 'args' is separated by '|' */

	char ifile[20];
	char ofile[20];

	char *former_args[MAX_LINE/2 + 1];
	char *latter_args[MAX_LINE/2 + 1];

	for (int i = 0; i < MAX_LINE/2 + 1; ++i) args[i] = NULL;
	cmd_history[0] = '\0';
	ifile[0] = '\0';
	ofile[0] = '\0';
	
    /* present the prompt repeated */
	while (should_run) {   
		printf("osh>");
		fflush(stdout);

	    /* present the prompt repeated */
		fgets(cmd, MAX_LINE, stdin);
		cmd[strlen(cmd) - 1] = '\0'; /* delete line-feed */

		/**
		 * After reading user input, the steps are:
		 * (1) fork a child process
		 * (2) the child process will invoke execvp()
		 * (3) if command included &, parent will invoke waitpid()
		 */

	    /* present the prompt repeated */
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

	    /* parse the command */
		wait_flag = parse_cmd(cmd, args, ifile, ofile, &split_pos);

		printf("Input file: %s\nOutput file: %s\n", ifile, ofile);

		printf("Split position: %d\n", split_pos);

		/* execute the command by creating a child process */
		exe(args, wait_flag, ifile, ofile, split_pos);

		/* clear values */
		for (int i = 0; i < MAX_LINE/2 + 1; ++i) 
		{	
			free(args[i]);
			args[i] = NULL;
		}
		ifile[0] = '\0';
		ofile[0] = '\0';

		split_pos = wait_flag = 0;
	}
    
	return 0;
}
