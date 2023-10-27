#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "export_builtin.h"

char * to_Substitute; 

char * string = (char *) 0;

bool is_Alpha_Numeric(char character) {
	if ( (48 <= character && character <= 57) || (65 <= character && character <= 90) || (97 <= character && character <= 122) ) {
		return true;
	}

	return false;
}

void clear_String() {
	if (string != (char *) 0) {
		free((void *) string);
		string = (char *) 0;
	}
}

//returns true if used getenv value
//if false, it is necessary to free this variable
bool getenv_With_Export_Override(char * env_Name, char ** env_Value) {
	*env_Value = query_Variable(env_Name);

	if (*env_Value == (char *) 0) {
		*env_Value = getenv(env_Name);
		return true;
	}

	return false;
}

void append_Env_To_String(char * env_Name) {
	char * env_Value;
	bool used_Getenv = getenv_With_Export_Override(env_Name, &env_Value);

	if (env_Value == (char *) 0) return;
	
	size_t len_String = (string == (char *) 0) ? 0: strlen(string);
	size_t len_Env = strlen(env_Value);
	
	char * new_String = (char *) malloc(sizeof(char) * (len_Env + len_String + 1));

	if (len_String != 0) {strcpy(new_String, string);} else {new_String[0] = '\0';}

	strcat(new_String, env_Value);
	
	if (!used_Getenv) free((void *) env_Value);

	clear_String();

	string = new_String;
}

void append_Char_To_String(char c) {
	size_t len =  (string == (char *) 0) ? 0: strlen(string);
	char * new_String = (char *) malloc(sizeof(char) * (len + 2));
	
	if (len != 0) strcpy(new_String, string);
	new_String[len + 1] = '\0';
	new_String[len] = c;

	clear_String();

	string = new_String;
}

char * get_Substring(char * string, int start_Index_Inclusive, int end_Index_Exclusive) {
	int char_Count = end_Index_Exclusive - start_Index_Inclusive;
	char * new_String = (char *) malloc(sizeof(char) * (char_Count + 1));
	
	strncpy(new_String, &string[start_Index_Inclusive], char_Count);
	new_String[char_Count] = '\0';

	return new_String;
}

void parse_Variables() {
	size_t len = strlen(to_Substitute);
	
	for (int i = 0; i < len;) {
		if (to_Substitute[i] != '$') {
			append_Char_To_String(to_Substitute[i]);
			i++;
			continue;

		} else {
			if (i >= 1) {
				if (to_Substitute[i - 1] == '\\') {
					append_Char_To_String(to_Substitute[i]);
					i++;
					continue;
				}	
			}
			
			i++;
			
			int start_Index_Inclusive = i;

			while (is_Alpha_Numeric(to_Substitute[i]) || to_Substitute[i] == '_') {
				i++;
			}
			
			int end_Index_Exclusive = i;

			char * env_Name = get_Substring(to_Substitute, start_Index_Inclusive, end_Index_Exclusive);

			append_Env_To_String(env_Name);
			free((char *) env_Name);
		}
	}
}

char * substitute(char * s) {
	to_Substitute = s;
	parse_Variables();
	
	char * r = string;
	string = (char *) 0;
	return r;
}
