/**
 * @file token.h
 * 
 * Parsing config tokens
 */

#pragma once
#ifndef __BB_CORE_CONFIG_TOKEN_HEADER__
#define __BB_CORE_CONFIG_TOKEN_HEADER__

#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  enum bbCoreConfigToken_t
  {
    BB_STRING = 257,
    BB_NUMBER,
    BB_SET,
    BB_ERROR,
  };

  struct bbCoreConfigTokenData_t
  {
    enum bbCoreConfigToken_t type;
    union 
    {
      char*  str;
      double num;
    };
  };

  void bbCoreConfigTokenReset(struct bbCoreConfigTokenData_t* tokenData);

  typedef void* yyscan_t;

  extern int yylex (yyscan_t yyscanner, locale_t locale);

  #define YY_DECL int yylex (yyscan_t yyscanner, locale_t locale)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BB_CORE_CONFIG_TOKEN_HEADER__ */