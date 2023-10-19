#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char * single_Quotes_To_String(char * string) {
        size_t length = strlen(string);
    
        char * new_String = (char *) malloc(sizeof(char) * (length - 1));
        strncpy(new_String, string + 1, length - 2);
        new_String[length - 2] = '\0';

        return new_String;
}

char * double_Quotes_To_String(char * string) {
	size_t length = strlen(string);
	char * buffer = (char *) malloc(sizeof(char) * (length - 1));

	int j = 0;
	for (int i = 1; i < length - 1; i++) {
		if (i != length - 2 && string[i] == '\\' && string[i + 1] == '"') {
			buffer[j++] = '"';
			i++;
			continue;
		}

		if (i != length - 2 && string[i] == '\\' && string[i + 1] == '\\') {
			buffer[j++] = '\\';
			i++;
			continue;
		}
		
		if (i != length - 2 && string[i] == '\\' && string[i + 1] == 'n') {
			buffer[j++] = '\n';
			i++;
			continue;
		}
		
		buffer[j++] = string[i];
	}

	char * new_String = (char *) malloc(sizeof(char) * (j + 1));
	strncpy(new_String, buffer, j);
	new_String[j] = '\0';

	free((void *) buffer);
	return new_String;
}


char * non_Quotes_To_String(char * string) {
        size_t length = strlen(string);
        char * buffer = (char *) malloc(sizeof(char) * (length + 1));

        int j = 0; 
        for (int i = 0; i < length; i++) {
                if (i != length - 1 && string[i] == '\\' && string[i + 1] == ' ') {
                        buffer[j++] = ' ';
                        i++;
                        continue;
                }

                if (i != length - 1 && string[i] == '\\' && string[i + 1] == '"') {
                        buffer[j++] = '"';
                        i++;
                        continue;
                }

		if (i != length - 1 && string[i] == '\\' && string[i + 1] == '\'') {
                        buffer[j++] = '\'';
                        i++;
                        continue;
                }


		if (i != length - 1 && string[i] == '\\' && string[i + 1] == '|') {
                        buffer[j++] = '|';
                        i++;
                        continue;
                }


		if (i != length - 1 && string[i] == '\\' && string[i + 1] == '\\') {
                        buffer[j++] = '\\';
                        i++;
                        continue;
                }

		if (i != length - 1 && string[i] == '\\' && string[i + 1] == 'n') {
                        buffer[j++] = '\n';
                        i++;
                        continue;
                }

		buffer[j++] = string[i];
        }

        char * new_String = (char *) malloc(sizeof(char) * (j + 1));
        strncpy(new_String, buffer, j);
        new_String[j] = '\0';

        free((void *) buffer);
        return new_String;
	
}

int get_Arguments_Length(char ** arguments) {
	int i;
	for (i = 0; arguments[i] != (char *)0 ; i++) {}

	return i;
}

int get_Commands_Length(char *** commands) {
	int i;
	for (i = 0; commands[i] != (char **) 0 ; i++) {}

	return i;
}

char ** create_Arguments_And_Destroy_Old(char ** arguments, char * string) {
	int arg_Length = get_Arguments_Length(arguments);

	char ** new_Arguments = (char **) malloc(sizeof(char *) * (arg_Length + 2));
	
	for (int i = 0; i < arg_Length; i++) {
		new_Arguments[i] = arguments[i];
	}

	new_Arguments[arg_Length] = string;
	new_Arguments[arg_Length + 1] = '\0';
	
	free((void *) arguments);

	return new_Arguments;
}

char ** create_Arguments_From_String(char * string) {
	char ** arguments = (char **) malloc(sizeof(char *) * 2);
	arguments[0] = string;
	arguments[1] = (char *) 0;

	return arguments;
}

char *** create_Commands_From_Arguments(char ** arguments) {
	char *** commands = (char ***) malloc(sizeof(char **) * 2);
	commands[0] = arguments;
	commands[1] = (char **) 0;
	
	return commands;
}

char *** create_Commands_And_Destroy_Old(char *** commands, char ** arguments) {
	int cmd_Length = get_Commands_Length(commands);

	char *** new_Commands = (char ***) malloc(sizeof(char **) * (cmd_Length + 2));
	
	for (int i = 0; i < cmd_Length; i++) {
		new_Commands[i] = commands[i];
	}

	new_Commands[cmd_Length] = arguments;
	new_Commands[cmd_Length + 1] = '\0';
	
	free((void *) commands);

	return new_Commands;
}

void debug_Print_All_Commands(char *** commands) {
	int command_Length = get_Commands_Length(commands);

	for (int i = 0; i < command_Length; i++) {
		char ** arguments = commands[i];
		int argument_Length = get_Arguments_Length(arguments);

		for (int j = 0; j < argument_Length; j++) {
			printf("%s ", arguments[j]);
		}

		printf("\n");
	}
}
