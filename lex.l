%{

#include <stdio.h>
#include <string.h>

#define NON_QUOTED_STRING 298
#define SINGLE_QUOTED_STRING 299
#define DOUBLE_QUOTED_STRING 300
#define PIPE 301
#define WHITE_SPACE 302

%}

%%

([^ \"'\|\\]|(\\[ ])|(\\\")|(\\')|(\\\|)|(\\\\))* { printf("NON_QUOTED_STRING "); return NON_QUOTED_STRING; }

'[^']*' {printf("SINGLE_QUOTED_STRING "); return SINGLE_QUOTED_STRING;}

\"([^\\\"]|(\\\")|(\\\\))*\" {printf("DOUBLE_QUOTED_STRING "); return DOUBLE_QUOTED_STRING;}

"|" { printf("PIPE "); return PIPE;}

[ \n\t]* {printf("WHITE_SPACE ");}

%%

int yywrap () {
	return 1;
}

/*

([^ \"'\|\\]|(\\[ ])|(\\\")|(\\')|(\\\|)|(\\\\))* { printf("NON_QUOTED_STRING "); return NON_QUOTED_STRING; }

All characters but spaces, double quotes, single quotes, pieps and escapes are allowed. They can only be used if escape character preceeds them.

'[^']*' {printf("SINGLE_QUOTED_STRING "); return SINGLE_QUOTED_STRING;}

All characters are allowed but single quotes.

\"([^\\\"]|(\\\")|(\\\\))*\" {printf("DOUBLE_QUOTED_STRING "); return DOUBLE_QUOTED_STRING;}

All characters are allowed except for quotes and escapes. If an escape character preceeds, quotes and escape characters are allowed.

"|" { printf("PIPE "); return PIPE;}

[ \n\t]* {printf("WHITE_SPACE ");}

*/
