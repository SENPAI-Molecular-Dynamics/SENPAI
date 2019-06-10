/*
 * bond.h
 *
 * Licensed under MIT license
 *
 */

#ifndef LENNARDJONES_H
#define LENNARDJONES_H

#include "universe.h"

vec3d_t *force_callback(vec3d_t *frc, t_universe *universe, const size_t part_id);
vec3d_t *force_bond(vec3d_t *frc, t_universe *universe, const size_t part_id);
vec3d_t *force_electrostatic(vec3d_t *frc, t_universe *universe, const size_t part_id);
vec3d_t *force_lennardjones(vec3d_t *frc, t_universe *universe, const size_t part_id);
vec3d_t *force_torsion(vec3d_t *frc, t_universe *universe, const size_t part_id);

#endif
