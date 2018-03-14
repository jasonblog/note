%{
void yyerror(const char *s);
%}
%%
program:
   ;
%%
void yyerror(const char *s)
{
}
int main()
{
    yyparse();
    return 0;
}
