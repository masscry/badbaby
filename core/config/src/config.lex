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
#include <ctype.h>

#include <token.h>

#ifndef __cplusplus
#define nullptr (0)
#endif /* __cplusplus */

%}

%%

\:                    {
                        yyextra->str = nullptr;
                        yyextra->type = BB_SET;
                        return BB_SET;
                      }

\{                    {
                        yyextra->str = nullptr;
                        yyextra->type = BB_START;
                        return BB_START;
                      }

\}                    {
                        yyextra->str = nullptr;
                        yyextra->type = BB_FINISH;
                        return BB_FINISH;
                      }

[-]*{D}+{E}?          {
                        yyextra->num = strtod_l(yytext, nullptr, locale);
                        yyextra->type = BB_NUMBER; 
                        return BB_NUMBER; 
                      }

[-]*{D}*"."{D}+{E}?   {
                        yyextra->num = strtod_l(yytext, nullptr, locale);
                        yyextra->type = BB_NUMBER;
                        return BB_NUMBER;
                      }

[-]*{D}+"."{D}*{E}?   {
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

.                     { /* SOME INVALID CHARACTER */ printf(isprint(*yytext)?"%c\n":"<%02X>\n", *yytext); assert(0); }

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
