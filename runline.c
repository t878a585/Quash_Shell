#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "parser_helper.h"

struct command {
	char * executable;
	char ** argv;
	int std, sti;	
};

struct commands {
	int count;
	struct command * commands;
};

void close_All_Pipes_But(int n, int * pipes, int pfd_One, int pfd_Two) {
	for (int i = 0; i < n*2; i++) {
		if (pipes[i] != pfd_One && pipes[i] != pfd_Two) {
			close(pipes[i]);
		}
	}
}

pid_t fork_And_Run(struct command c, int pipes_Count, int * pipes) {
	pid_t p = fork();
	
	if (p == 0) {
		if (c.std != 1 && c.sti != 0) {
			dup2(c.std, 1);
			dup2(c.sti, 0);
		}
		
		if (pipes != (int *)0) {
			close_All_Pipes_But(pipes_Count, pipes, c.std, c.sti);
			free(pipes);
		}
		
		execvp(c.executable, c.argv);
	} else {
		return p;
	}
}

void create_Pipes(int n, int * pipes) {
	for (int i = 0; i < n; i++) {
		pipe(pipes + (i * 2));
	}
}

void close_Pipes(int n, int * pipes) {
	for (int i = 0; i < n * 2; i++) {
		close(pipes[i]);
	}
}

int * create_And_Allocate_Pipes(int n) {
	int * pipes = (int *) malloc(sizeof(int) * (2*n));

	create_Pipes(n, pipes);
	
	return pipes;
}

pid_t * fork_And_Run_All(struct commands * cs) {
	pid_t * cpids = (pid_t *) malloc(sizeof(pid_t) * (cs->count));

	if (cs->count == 1) {
		(cs->commands[0]).std = 1;
		(cs->commands[0]).sti = 0;
		
		cpids[0] = fork_And_Run(cs->commands[0], 0, (int *) 0);
		
		return cpids;
	}
	
	int pipe_Count = cs->count - 1;
	int * pipes = create_And_Allocate_Pipes(pipe_Count);
	
	for (int i = 0; i < cs->count; i++) {
		if (i != cs->count - 1) {(cs->commands[i]).std = pipes[1 + (i * 2)];}

		if (i != 0) {(cs->commands[i]).sti = pipes[(i - 1) * 2];}
	}
	
	for (int i = 0; i < cs->count; i++) {
		cpids[i] = fork_And_Run(cs->commands[i], pipe_Count, pipes); 
	}

	close_Pipes(pipe_Count, pipes);
	free(pipes);

	return cpids;
}

void runcmds(struct commands * cs) {
	pid_t * pids = fork_And_Run_All(cs);
	
	for (int i = 0; i < cs->count; i++) {
		waitpid(pids[i], NULL, 0);	
	}

	free((void *) pids);
	
	return;
}

struct command arguments_To_Command(char ** arguments) {
	struct command c = {.executable=arguments[0], .argv=arguments};
	
	return c;
}

void free_Commands_Struct(struct commands * cs) {
	free((void *) cs->commands);
}


void free_Commands(char *** commands) {
	for (int i = 0; commands[i] != (char **) 0; i++) {
		char ** arguments = commands[i];

		for (int j = 0; arguments[j] != (char *) 0; j++) {
			free((void *) arguments[j]);
		}

		free((void *) arguments);
	}

	free((void *) commands);
}

struct commands to_Struct_Format(char *** commands) {
	struct commands cs;
	int commands_Length = get_Commands_Length(commands);
	
	cs.count = commands_Length;	
	cs.commands = (struct command *) malloc(sizeof(struct command) * commands_Length);

	for (int i = 0; i < commands_Length; i++) {
		char ** arguments = commands[i];
		struct command c = arguments_To_Command(arguments);
		cs.commands[i] = c;
	}

	return cs;
}

void execute_And_Free_Commands(char *** commands) {
	struct commands cs = to_Struct_Format(commands);
	runcmds(&cs);
	free_Commands_Struct(&cs);
	free_Commands(commands);
}
