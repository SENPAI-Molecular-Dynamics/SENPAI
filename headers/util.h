/*
 * util.h
 *
 * Licensed under MIT license
 *
 */

#ifndef UTIL_H
#define UTIL_H

#define C_AVOGADRO (double)(6.02214076E23)
#define C_IDEALGAS (double)(8.31446261)
#define C_ELEC     (double)(8.98755*10E9)

void *retstr(void *ret, const char *str, const char *file, const int line);
int   retstri(int ret, const char *str, const char *file, const int line);

#endif
