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

	close_If_Not_Stdio(c->sti);
	close_If_Not_Stdio(c->std);
}

void * pwd(void * args) {
	struct command * c = (struct command *) args;
	close_If_Not_Stdio(c->sti);
	FILE * stdout_File = fdopen(c->std, "a");

	char * current_Directory = getcwd((char *) 0, 0);

	fprintf(stdout_File, "%s\n", current_Directory);
	fflush(stdout_File);

	free((void *) current_Directory);
	close_If_Not_Stdio(c->std);
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

char * hooked[] = {"echo", "pwd", "cd"};
void * (*hooked_Functions[])(void *) = {echo, pwd, cd};

//Returns true if it was a hookable command
bool hook(struct command * c) {
	void * (* function)(void *);
	bool proceed = false;

	for (int i = 0; i < sizeof(hooked)/sizeof(hooked[0]); i++) {
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