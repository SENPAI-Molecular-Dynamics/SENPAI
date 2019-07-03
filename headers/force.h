/*
 * force.h
 *
 * Licensed under MIT license
 *
 */

#ifndef FORCE_H
#define FORCE_H

#include "universe.h"
#include "vec3d.h"

universe_t *force_bond(vec3d_t *frc, universe_t *universe, const size_t p1, const size_t p2);
universe_t *force_electrostatic(vec3d_t *frc, universe_t *universe, const size_t p1, const size_t p2);
universe_t *force_lennardjones(vec3d_t *frc, universe_t *universe, const size_t p1, const size_t p2);
universe_t *force_total(vec3d_t *frc, universe_t *universe, const size_t part_id);

#endif