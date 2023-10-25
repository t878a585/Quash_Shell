#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shared_datatypes.h"

pthread_t * threads;
int thread_Count = 0;

void append_Thread(pthread_t thread) {
	pthread_t * tmp = (pthread_t *) malloc(sizeof(pthread_t) * (++thread_Count));

	for (int i = 0; i < thread_Count - 1; i++) {
		tmp[i] = threads[i]; 
	}
	
	tmp[thread_Count - 1] = thread;

	//If previously held zero elements, we know we don't need to free
	if (thread_Count > 1) free((void *) threads);

	threads = tmp;
}

void close_If_Not_Stdio(int fd) {
	if (fd != 0 && fd != 1) close(fd);
}

void clear_Threads() {
	free((void *) threads);
	thread_Count = 0;
}

void * echo(void * args) {
	struct command * c = (struct command *) args;
	FILE * stdout_File = fdopen(c->std, "a");
		
	for (int i = 1; c->argv[i] != (char *) 0; i++) {
		if (i == 1) {fprintf(stdout_File, "%s", c->argv[i]);} else {
			fprintf(stdout_File, " %s", c->argv[i]);
		}
		
	}
	
	fprintf(stdout_File, "\n");

	close_If_Not_Stdio(c->sti);
	close_If_Not_Stdio(c->std);
}

void * pwd(void * args) {
	struct command * c = (struct command *) args;
	FILE * stdout_File = fdopen(c->std, "a");

	char * current_Directory = getcwd((char *) 0, 0);

	fprintf(stdout_File, "%s\n", current_Directory);
	fflush(stdout_File);

	free((void *) current_Directory);
	close_If_Not_Stdio(c->std);
	close_If_Not_Stdio(c->sti);
}

void * cd(void * args) {
	struct command * c = (struct command *) args;
	if (c->argv[1] == (char *) 0) return;
		
	if (chdir(c->argv[1])) {
		FILE * stdout_File = fdopen(c->std, "a");
		
		fprintf(stdout_File, "Could not change directory\n");
	}
	

	close_If_Not_Stdio(c->sti);
	close_If_Not_Stdio(c->std);
}


void terminate() {
	exit(0);
}

char * hooked[] = {"echo", "pwd", "cd"};
void * (*hooked_Functions[])(void *) = {echo, pwd, cd};
const int hooked_Size = sizeof(hooked)/sizeof(hooked[0]);

char * early_Threadless_Hooked[] = {"quit", "exit"};
void (*early_Threadless_Hooked_Functions[])() = {terminate, terminate};
const int early_Threadless_Hooked_Size = sizeof(early_Threadless_Hooked)/sizeof(early_Threadless_Hooked[0]);

void early_Threadless_Hook(struct commands * cs) {
	for (int i = 0; i < cs->count; i++) {
		struct command * c = &cs->commands[i];

		for (int j = 0; j < early_Threadless_Hooked_Size; j++) {
			if (!strcmp(c->executable, early_Threadless_Hooked[j])) {
				early_Threadless_Hooked_Functions[j]();
				return;
			}
		}
	}
}

//Returns true if it was a hookable command
bool hook(struct command * c) {
	void * (* function)(void *);
	bool proceed = false;

	for (int i = 0; i < hooked_Size; i++) {
		if (!strcmp(c->executable, hooked[i])) {
			proceed = true;

			function = hooked_Functions[i];

			break;
		}
	}
	
	if (!proceed) return false;
	
	pthread_t thread;
	
	pthread_create(&thread, NULL, function, (void *) c);

	append_Thread(thread);

	return true;
}

void wait_On_Hooked() {
	if (thread_Count == 0) return;
	
	for (int i = 0; i < thread_Count; i++) {pthread_join(threads[i], NULL);}

	clear_Threads();
}
