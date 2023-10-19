#ifndef PARSER_HELPER_H
#define PARSER_HELPER_H

char * single_Quotes_To_String(const char * string);

char * double_Quotes_To_String(const char * string);

char * non_Quotes_To_String(const char * string);

char ** create_Arguments_And_Destroy_Old(char ** arguments, char * string);

char ** create_Arguments_From_String(char * string);

char *** create_Commands_From_Arguments(char ** arguments);

char *** create_Commands_And_Destroy_Old(char *** commands, char ** arguments);

void debug_Print_All_Commands(char *** commands);

int get_Commands_Length(char *** commands);

#endif
