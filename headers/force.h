/*
 * force.h
 *
 * Licensed under MIT license
 *
 */

#ifndef FORCE_H
#define FORCE_H

#include "universe.h"

universe_t *force_callback(universe_t *universe, const size_t part_id);
universe_t *force_bond(universe_t *universe, const size_t part_id);
universe_t *force_electrostatic(universe_t *universe, const size_t part_id);
universe_t *force_lennardjones(universe_t *universe, const size_t part_id);
universe_t *force_torsion(universe_t *universe, const size_t part_id);

#endif