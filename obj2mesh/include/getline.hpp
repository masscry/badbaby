#pragma once
#ifndef __BB_OBJ2MESH_GETLINE_HEADER__
#define __BB_OBJ2MESH_GETLINE_HEADER__

#ifndef _WIN32
#error "This functions implements compitability layer for non-POSIX platform, do not use elsewhere"
#endif

#ifdef __cplusplus
#define BB_CAPI extern "C"
#endif

#include <cstdio>
#include <cstdlib>
#include <basetsd.h>

using ssize_t = SSIZE_T;

BB_CAPI ssize_t getdelim(char **buf, size_t *bufsiz, int delimiter, FILE *fp);
BB_CAPI ssize_t getline(char **buf, size_t *bufsiz, FILE *fp);

#endif /* __BB_OBJ2MESH_GETLINE_HEADER__ */