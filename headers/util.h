/*
 * util.h
 *
 * Licensed under MIT license
 *
 */

#ifndef UTIL_H
#define UTIL_H

#define ROOT_MACHINE_EPSILON (double) 1.48996644E-8

#define C_AVOGADRO (double)(6.02214076E23)
#define C_IDEALGAS (double)(8.31446261)
#define C_ELEC     (double)(8.98755*10E9)

#define POW3(x) (x*x*x)
#define POW6(x) (POW3(x)*POW3(x))
#define POW12(x) (POW6(x)*POW6(x))

void *retstr(void *ret, const char *str, const char *file, const int line);
int retstri(int ret, const char *str, const char *file, const int line);
double retstrf(double ret, const char *str, const char *file, const int line);

#endif
