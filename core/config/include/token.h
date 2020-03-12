/**
 * @file token.h
 *
 * Parsing config tokens
 */

#pragma once
#ifndef __BB_CORE_CONFIG_TOKEN_HEADER__
#define __BB_CORE_CONFIG_TOKEN_HEADER__

#include <locale.h>
#ifdef __APPLE__
#include <xlocale.h>
#endif /* __APPLE__ */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef _WIN32

  typedef _locale_t locale_t;

  locale_t newlocale(int mask, const char* locale, locale_t);

  void freelocale(locale_t locale);

  double strtod_l(const char* strSource, char** endptr, locale_t locale);

#define LC_COLLATE_MASK _M_COLLATE
#define LC_CTYPE_MASK _M_CTYPE
#define LC_MONETARY_MASK _M_MONETARY
#define LC_NUMERIC_MASK _M_NUMERIC
#define LC_TIME_MASK _M_TIME
#define LC_MESSAGES_MASK _M_MESSAGES
#define LC_ALL_MASK (  LC_COLLATE_MASK \
                     | LC_CTYPE_MASK \
                     | LC_MESSAGES_MASK \
                     | LC_MONETARY_MASK \
                     | LC_NUMERIC_MASK \
                     | LC_TIME_MASK )

#ifndef strdup
#define strdup _strdup
#endif

#endif

  enum bbCoreConfigToken_t
  {
    BB_STRING = 257,
    BB_NUMBER,
    BB_SET,
    BB_START,
    BB_FINISH,
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

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
  typedef void* yyscan_t;
#endif /* YY_TYPEDEF_YY_SCANNER_T */

  extern int yylex (yyscan_t yyscanner, locale_t locale);

  #define YY_DECL int yylex (yyscan_t yyscanner, locale_t locale)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BB_CORE_CONFIG_TOKEN_HEADER__ */
