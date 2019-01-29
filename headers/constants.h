/*
 * constants.h
 *
 * Licensed under MIT license
 *
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define C_MAXTIME 1 /* Stop simulation at 1 second */

/* All values are given in SI units */
#define C_GRAV_DEFAULT (double)6.67408E-11   /* Gravitational constant */
#define C_ELEC_DEFAULT (double)8.98755E+09   /* Coulomb's constant */
#define C_TIME_DEFAULT (double)1E-9          /* dt */
#define C_SIZE_DEFAULT (double)1E0           /* Radius of the simulated universe (sphere) */

#define C_PART_NB   (long int)1024
#define C_MASS_PROT
#define C_MASS_NEUT
#define C_MASS_ELEC

#endif
