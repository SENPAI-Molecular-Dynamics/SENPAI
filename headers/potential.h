/*
 * potential.h
 *
 * Licensed under GPLv3 license
 *
 */

#ifndef POTENTIAL_H
#define POTENTIAL_H

#include <stdint.h>

#include "universe.h"

universe_t *potential_bond(double *pot, universe_t *universe, const uint64_t a1, const uint64_t a2);
universe_t *potential_electrostatic(double *pot, universe_t *universe, const uint64_t a1, const uint64_t a2);
universe_t *potential_lennardjones(double *pot, universe_t *universe, const uint64_t a1, const uint64_t a2);
universe_t *potential_angle(double *pot, universe_t *universe, const uint64_t a1, const uint64_t a2);
universe_t *potential_total(double *pot, universe_t *universe, const uint64_t atom_id);

#endif
