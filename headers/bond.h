/*
 * bond.h
 *
 * Licensed under MIT license
 *
 */

#ifndef LENNARDJONES_H
#define LENNARDJONES_H

#include <universe.h>

double lj_epsilon(const t_particle *p1, const t_particle *p2);
double lj_sigma(const t_particle *p1, const t_particle *p2);
double lennardjones(const t_particle *p1, const t_particle *p2);

double bond_force(const size_t bond_id, const t_particle *p);

#endif
