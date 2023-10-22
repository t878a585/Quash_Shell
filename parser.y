%{

#include <stdio.h>
#include <stdlib.h>
#include "parser_helper.h"
#include "runline.h"

extern int yyflex();
char *** parsed_Commands;

void yyerror(const char * s) {return;}

%}

%token NON_QUOTED_STRING
%token SINGLE_QUOTED_STRING
%token DOUBLE_QUOTED_STRING
%token PIPE

%start start

%union {
	char *** commands;
	char ** arguments;
	char * string;
}

%type<string> NON_QUOTED_STRING SINGLE_QUOTED_STRING DOUBLE_QUOTED_STRING string 
%type<arguments> arguments command
%type<commands> commands start

%%

start: commands {parsed_Commands = $1;}
;

commands: commands PIPE command {$$ = create_Commands_And_Destroy_Old($1, $3);} 
	| command {$$ = create_Commands_From_Arguments($1);}
;

command: arguments {$$ = $1;}
;

arguments: arguments string {$$ = create_Arguments_And_Destroy_Old($1, $2);}
	| string {$$ = create_Arguments_From_String($1);} 
;

string: NON_QUOTED_STRING {$$ = non_Quotes_To_String($1); free($1);}
	| SINGLE_QUOTED_STRING {$$ = single_Quotes_To_String($1); free($1);}
	| DOUBLE_QUOTED_STRING {$$ = double_Quotes_To_String($1); free($1);}
;

%%

int main () {
	while(1) {
		printf("[QUASH]$ ");
		yyparse();

		if (parsed_Commands == (char ***) 0) continue;

		execute_And_Free_Commands(parsed_Commands);
	}
	
}
