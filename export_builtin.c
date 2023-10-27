#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "shared_datatypes.h"

struct Variable {
	char * var;
	char * value;
};

struct Variable * variables = (struct Variable *) 0;
int variable_Count = 0;
pthread_mutex_t export_Variables_Lock;
bool export_Variables_Lock_Initialized = false;

void export_Init() {
	if (!export_Variables_Lock_Initialized) pthread_mutex_init(&export_Variables_Lock, NULL);
}

char * query_Variable(char * variable) {
	pthread_mutex_lock(&export_Variables_Lock);
	
	struct Variable * v;

	for (int i = 0; i < variable_Count; i++) {
		if (!strcmp(variable, v[i].var)) {
			char * return_Value = strdup(v[i].value);

			pthread_mutex_unlock(&export_Variables_Lock);

			return return_Value;
		}
	}

	pthread_mutex_unlock(&export_Variables_Lock);

	return (char *) 0;
}

void add_Variable(char * variable, char * value) {
	pthread_mutex_lock(&export_Variables_Lock);
	struct Variable * v;

	for (int i = 0; i < variable_Count; i++) {
		v = &variables[i];

		if (!strcmp(variable, v->var)) {
			free((void *) v->value);
			v->value = strdup(value);
			
			pthread_mutex_unlock(&export_Variables_Lock);
			return;
		}
	}

	struct Variable * variables_New = (struct Variable *) malloc(sizeof(struct Variable *) * variable_Count + 1);
	
	for (int i = 0; i < variable_Count; i++) {
		variables_New[i].var = variables[i].var;
		variables_New[i].value = variables[i].value;
	}

	free((void *) variables);
	variables = variables_New;

	variable_Count++;

	variables[variable_Count - 1].var = strdup(variable);
	variables[variable_Count - 1].value = strdup(value);

	pthread_mutex_unlock(&export_Variables_Lock);
}

size_t get_Total_Command_Length(char ** arguments) {
	size_t len = 0;

	for (int i = 0; arguments[i] != (char *) 0; i++) {
		len += strlen(arguments[i]);
	}

	return len;
}

static void close_If_Not_Stdio(int fd) {
	if (fd != 0 && fd != 1) close(fd);
}


void * export(void * args) {
	struct command * c = (struct command *) args;
	char * set_Var_String = strdup(c->argv[1]);
	char * var, * value;

	var = strtok(set_Var_String, "=");
	value = strtok(NULL, "=");

	add_Variable(var, value);

	free((void *) set_Var_String);

	close_If_Not_Stdio(c->sti);
	close_If_Not_Stdio(c->std);
}

void set_Exports_In_Current_Process() {
	pthread_mutex_lock(&export_Variables_Lock);

	for (int i = 0; i < variable_Count; i++) {
		struct Variable * var = &variables[i];
		setenv(var->var, var->value, 1);
	}

	pthread_mutex_unlock(&export_Variables_Lock);
}
