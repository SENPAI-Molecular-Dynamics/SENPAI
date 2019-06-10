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

double potential_callback(universe_t *universe, const size_t part_id, uint8_t *err_flag);
double potential_bond(universe_t *universe, const size_t part_id, uint8_t *err_flag);
double potential_electrostatic(universe_t *universe, const size_t part_id, uint8_t *err_flag);
double potential_lennardjones(universe_t *universe, const size_t part_id, uint8_t *err_flag);
double potential_torsion(universe_t *universe, const size_t part_id, uint8_t *err_flag);

#endif
