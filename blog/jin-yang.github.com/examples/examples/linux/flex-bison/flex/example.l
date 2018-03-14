%{
#include <stdio.h>
%}
%%
is |
are                     { printf("%s: VERB ", yytext); }
island                  printf("LAND ");
[0-9]+                  printf("NUMBER ");
[ \t]+                  /* ignore whitespace */;
[a-zA-Z][a-zA-Z0-9]*    printf("COMMON WORD ");
.|\n                    { ECHO; }
%%
int main(void) {
    yylex();
}
