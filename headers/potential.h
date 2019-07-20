/*
 * potential.h
 *
 * Licensed under MIT license
 *
 */

#ifndef POTENTIAL_H
#define POTENTIAL_H

#include <stdint.h>

#include "universe.h"

universe_t *potential_bond(double *pot, universe_t *universe, const size_t a1, const size_t a2);
universe_t *potential_electrostatic(double *pot, universe_t *universe, const size_t a1, const size_t a2);
universe_t *potential_lennardjones(double *pot, universe_t *universe, const size_t a1, const size_t a2);
universe_t *potential_angle(double *pot, universe_t *universe, const size_t a1, const size_t a2);
universe_t *potential_total(double *pot, universe_t *universe, const size_t atom_id);

#endif
