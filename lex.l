%{

#include <stdio.h>
#include "parser.tab.h"

%}

%%

([^ \"'\|\\\n]|(\\[ ])|(\\\")|(\\')|(\\\|)|(\\\\)|(\\n))* { yylval.string = strdup(yytext); return NON_QUOTED_STRING; }

'[^'\n]*' { yylval.string = strdup(yytext); return SINGLE_QUOTED_STRING;}

\"([^\\\"\n]|(\\\")|(\\\\)|(\\n))*\" { yylval.string = strdup(yytext); return DOUBLE_QUOTED_STRING;}

"|" { return PIPE;}

[ \t]* {}

[\n] { return 0;}

%%

int yywrap () {
	return 1;
}

/*


([^ \"'\|\\\n]|(\\[ ])|(\\\")|(\\')|(\\\|)|(\\\\)|(\\n))* { printf("NON_QUOTED_STRING");yylval.string = strdup(yytext); return NON_QUOTED_STRING; }

All characters but spaces, double quotes, single quotes, pipes, escapes, and newlines are allowed. They can only be used if escape character preceeds them except with with newlines which require an escape before an n character.

'[^']*' {printf("SINGLE_QUOTED_STRING "); return SINGLE_QUOTED_STRING;}

All characters are allowed but single quotes.

\"([^\\\"\n]|(\\\")|(\\\\)|(\\n))*\" {printf("DOUBLE_QUOTED_STRING "); return DOUBLE_QUOTED_STRING;}

All characters are allowed except for quotes, escapes, and newlines. If an escape character preceeds, quotes and escape characters are allowed. If '\n' is present, it will be interpretted as a newline.

"|" { printf("PIPE "); return PIPE;}

[ \n\t]* {printf("WHITE_SPACE ");}

*/
