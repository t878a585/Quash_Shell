#include <unistd.h>

struct command {
	char * executable;
	char ** argv;
	int std, sti;	
};

struct commands {
	int count;
	struct command * commands;
};

pid_t fork_And_Run(struct * command) {
	pid_t p = fork();

	if (p == 0) {
		if (std != 1 && sti != 0) {
			dup(command->std, 1);
			dup(command->sti, 0);
		}

		execvp(command->executable, command->argv);
	} else {
		return p;
	}
}

pid_t * fork_And_Run_All(struct commands * cs) {
	if (cs->count == 1) {
		pid_t * cpids = (pid_t *) malloc(sizeof(pid_t) * 2);
		cpids[1] = (pid_t *) 0;
		
		(cs->commands[0]).std = 1;
		(cs->commands[0]).std = 0;
		
		cpids[0] = fork_And_Run(cs->commands[0]);
		
		return cpids;
	}
	
	const int pipe_Count = cs->count - 1;
	cpids = (pid_t *) malloc(sizeof(pid_t) * (pipe_Count));
	int * pipes = malloc(sizeof(int) * (2 * (pipe_Count)));
	
	for (int i = 0; i < pipe_Count; i++) {
		pipe(&pipes[i * 2]);

	}

}

void runcmds(struct * piped_Commands) {
	
	fork();
}
