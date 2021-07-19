/*
 * util.h
 *
 * Licensed under GPLv3 license
 *
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

/* Useful constants for chemists */

/* Raises x to the corresponding power */
#define POW2(x)  (x*x)
#define POW3(x)  (x*POW2(x))
#define POW6(x)  (POW3(x)*POW3(x))
#define POW7(x)  (x*POW6(x))
#define POW12(x) (POW6(x)*POW6(x))
#define POW13(x) (x*POW12(x))

/* Prints str, with extra info (__FILE__ and __LINE__) before returning ret */
void *retstr(void *ret, const char *str, const char *file, const int line);
int retstri(const int ret, const char *str, const char *file, const int line);
double retstrf(const double ret, const char *str, const char *file, const int line);

/* Returns the number of lines in str */
uint64_t line_nb(const char *str);

#endif
