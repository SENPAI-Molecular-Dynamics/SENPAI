/*
 * force.h
 *
 * Licensed under GPLv3 license
 *
 */

#ifndef FORCE_H
#define FORCE_H

#include "universe.h"
#include "vec3.h"

universe_t *force_bond(vec3_t *frc, universe_t *universe, const uint64_t a1, const uint64_t a2);
universe_t *force_electrostatic(vec3_t *frc, universe_t *universe, const uint64_t a1, const uint64_t a2);
universe_t *force_lennardjones(vec3_t *frc, universe_t *universe, const uint64_t a1, const uint64_t a2);
universe_t *force_angle(vec3_t *frc, universe_t *universe, const uint64_t a1, const uint64_t a2);
universe_t *force_total(vec3_t *frc, universe_t *universe, const uint64_t atom_id);

#endif