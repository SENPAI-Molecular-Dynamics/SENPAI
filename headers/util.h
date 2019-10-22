/*
 * util.h
 *
 * Licensed under MIT license
 *
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define ROOT_MACHINE_EPSILON (double) 1.48996644E-8
#define LENNARDJONES_CUTOFF 2.5
#define MODE_ANALYTICAL 0
#define MODE_NUMERICAL 1

/* Useful constants for chemists */
#define C_BOLTZMANN  (double)(1.380649E-23)     /* Boltzmann constant */
#define C_AVOGADRO   (double)(6.02214076E23)    /* Avogadro number */
#define C_IDEALGAS   (double)(8.31446261)       /* Ideal gas constant */
#define C_VACUUMPERM (double)(8.8541878128E-12) /* Vacuum permitivity */
#define C_COULOMB    (double)(8.98755*10E9)     /* Coulomb constant */
#define C_AHO        (double)(5E-18)            /* Torsion constant (Nm/rad) */

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
