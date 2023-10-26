#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

#include "shared_datatypes.h"

#include "parser_helper.h"
#include "builtin.h"
#include "export_builtin.h"

void close_All_Pipes_But(int n, int * pipes, int pfd_One, int pfd_Two) {
	for (int i = 0; i < n*2; i++) {
		if (pipes[i] != pfd_One && pipes[i] != pfd_Two) {
			close(pipes[i]);
		}
	}
}

bool * allocate_Pipes_Keep_Open(int pipes_Count) {
	bool * keep_Open = malloc(sizeof(bool) * pipes_Count);

	for (int i = 0; i < pipes_Count; i++) {
		keep_Open[i] = false;
	}

	return keep_Open;
}

pid_t fork_And_Run(struct command * c, int pipes_Count, int * pipes, bool * pipes_Keep_Open) {
	if (hook(c)) {
		if (pipes_Count == 0) return 0;

		for (int i = 0; i < pipes_Count * 2; i++) {
			if (pipes[i] == c->std || pipes[i] == c->sti) {
				pipes_Keep_Open[i] = true;
			}
		}

		return 0;
	}

	pid_t p = fork();
	
	if (p == 0) {
		if (c->std != 1) dup2(c->std, 1);
		if (c->sti != 0) dup2(c->sti, 0);
		
		
		if (pipes != (int *)0) {
			close_All_Pipes_But(pipes_Count, pipes, c->std, c->sti);
			free(pipes);
		}
		
		set_Exports_In_Current_Process();
		
		execvp(c->executable, c->argv);
	} else {
		return p;
	}
}

void create_Pipes(int n, int * pipes) {
	for (int i = 0; i < n; i++) {
		pipe(pipes + (i * 2));
	}
}

void close_Pipes(int n, int * pipes, bool * keep_Pipes_Open) {
	for (int i = 0; i < n * 2; i++) {
		if(!keep_Pipes_Open[i]) close(pipes[i]);
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
		cpids[0] = fork_And_Run(&cs->commands[0], 0, (int *) 0, 0);
		
		return cpids;
	}
	
	int pipe_Count = cs->count - 1;
	int * pipes = create_And_Allocate_Pipes(pipe_Count);
	bool * pipes_Keep_Open = allocate_Pipes_Keep_Open(pipe_Count);
	
	for (int i = 0; i < cs->count; i++) {
		if (i != cs->count - 1) {(cs->commands[i]).std = pipes[1 + (i * 2)];}

		if (i != 0) {(cs->commands[i]).sti = pipes[(i - 1) * 2];}
	}
	
	for (int i = 0; i < cs->count; i++) {
		cpids[i] = fork_And_Run(&cs->commands[i], pipe_Count, pipes, pipes_Keep_Open); 
	}

	close_Pipes(pipe_Count, pipes, pipes_Keep_Open);
	free((void *) pipes_Keep_Open);
	free((void *) pipes);

	return cpids;
}

void runcmds(struct commands * cs) {
	pid_t * pids = fork_And_Run_All(cs);
	
	wait_On_Hooked();

	for (int i = 0; i < cs->count; i++) {
		if (pids[i] != 0) waitpid(pids[i], NULL, 0);	
	}

	free((void *) pids);
	
	return;
}

struct command arguments_To_Command(char ** arguments) {
	struct command c = {.executable=arguments[0], .argv=arguments, .sti=0, .std=1};
	
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
	early_Threadless_Hook(&cs);
	runcmds(&cs);
	free_Commands_Struct(&cs);
	free_Commands(commands);
}
