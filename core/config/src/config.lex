%option nounput
%option noinput
%option 8bit
%option never-interactive
%option noyywrap
%option reentrant
%option nodefault
%option extra-type="struct bbCoreConfigTokenData_t *"

D    [0-9]
E    ([Ee][+-]?{D}+)

%{

#include <assert.h>
#include <stdlib.h>

#include <token.h>

#ifndef __cplusplus
#define nullptr (0)
#endif /* __cplusplus */

%}

%%

[:]                   {
                        yyextra->str = nullptr;
                        yyextra->type = BB_SET;
                        return BB_SET;
                      }

{D}+{E}?              {
                        yyextra->num = strtod_l(yytext, nullptr, locale);
                        yyextra->type = BB_NUMBER; 
                        return BB_NUMBER; 
                      }

{D}*"."{D}+{E}?       {
                        yyextra->num = strtod_l(yytext, nullptr, locale);
                        yyextra->type = BB_NUMBER;
                        return BB_NUMBER;
                      }

{D}+"."{D}*{E}?       {
                        yyextra->num = strtod_l(yytext, nullptr, locale);
                        yyextra->type = BB_NUMBER;
                        return BB_NUMBER;
                      }

\"[^\"]*\"            { 
                        yyextra->str = strdup(yytext+1);
                        yyextra->str[strlen(yyextra->str)-1] = 0;
                        yyextra->type = BB_STRING;
                        return BB_STRING;
                      }
                      
[[:space:]]+          { /* IGNORE SPACES */ }

<<<<<<< HEAD
[[:space:]]+          { /* SKIP SPACES */ }

.                     { /* DO NOTHING */ fprintf(stderr, "GOT %c (%d)\n", *yytext, *yytext); assert(0); }
=======
.                     { /* SOME INVALID CHARACTER */  assert(0); }
>>>>>>> 96ce22f48b377f65755167e71a4d8fa7f78e5dfd

%%

void bbCoreConfigTokenReset(struct bbCoreConfigTokenData_t* tokenData)
{
  if (tokenData->type == BB_STRING)
  {
    free(tokenData->str);
  }
  tokenData->type = BB_ERROR;
  tokenData->str = nullptr;
}
