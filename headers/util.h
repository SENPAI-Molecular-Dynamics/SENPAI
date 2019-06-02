/*
 * util.h
 *
 * Licensed under MIT license
 *
 */

#ifndef UTIL_H
#define UTIL_H

#define C_AVOGADRO 6.02214076E23
#define C_IDEALGAS 8.31446261

void *retstr(void *ret, const char *str, const char *file, const int line);
int   retstri(int ret, const char *str, const char *file, const int line);

#endif
