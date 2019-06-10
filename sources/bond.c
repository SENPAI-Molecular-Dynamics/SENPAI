/*
 * lennard-jones.c
 *
 * Licensed under MIT license
 *
 */

#include <stdio.h>

#include "text.h"
#include "util.h"
#include "vec3d.h"
#include "universe.h"
#include "bond.h"

/* Universe boundaries reaction force */
vec3d_t *force_callback(vec3d_t *frc, t_universe *universe, const size_t part_id)
{
}

/* Covalent interaction */
vec3d_t *force_bond(vec3d_t *frc, t_universe *universe, const size_t part_id)
{
  size_t i;
  double dst_mag;
  double displacement;
  vec3d_t dst;

  frc->x = 0.0;
  frc->y = 0.0;
  frc->z = 0.0;

  for (i=0; i<7; ++i)
  {
    if (universe->particle[part_id].bond[i] != NULL)
    {
      /* Get the position difference vector */
      if (vec3d_sub(&dst, &(universe->particle[part_id].pos), &(universe->particle[i].pos)) == NULL)
        return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

      /* Get the distance between the two particles */
      if ((dst_mag = vec3d_mag(&dst)) < 0.0)
        return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

      /* Compute the bond force */
      displacement = (dst_mag - universe->particle[part_id].bond_length[i])/dst_mag;
      if (vec3d_mul(&dst, &dst, (universe->particle[part_id].bond_strength[i])*displacement) == NULL)
        return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));
      
      /* Sum it */
      if (vec3d_add(frc, frc, &dst) == NULL)
        return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));
    }
  }
  
  return (frc);
}

/* Electrostatic interaction */
vec3d_t *force_electrostatic(vec3d_t *frc, t_universe *universe, const size_t part_id)
{
  size_t i;
  double dst_mag;
  vec3d_t dst;

  frc->x = 0.0;
  frc->y = 0.0;
  frc->z = 0.0;

  for (i=0; i<(universe->part_nb); ++i)
  {
    if (i != part_id)
    {
      /* Get the position difference vector */
      if (vec3d_sub(&dst, &(universe->particle[part_id].pos), &(universe->particle[i].pos)) == NULL)
        return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));

      /* Get the distance between the two particles */
      if ((dst_mag = vec3d_mag(&dst)) < 0.0)
        return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));

      /* Compute the force */
      if (vec3d_mul(&dst, &dst, C_ELEC*(universe->particle[part_id].charge)*(universe->particle[i].charge)/(dst_mag*dst_mag*dst_mag)) == NULL)
        return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));

      /* Sum it */
      if (vec3d_add(frc, frc, &dst) == NULL)
        return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));
    }
  }
  
  return (frc);
}

/* Van der Waals interaction */
vec3d_t *force_lennardjones(vec3d_t *frc, t_universe *universe, const size_t part_id)
{
  frc->x = 0.0;
  frc->y = 0.0;
  frc->z = 0.0;
}

/* Bond torsion interaction */
vec3d_t *force_torsion(vec3d_t *frc, t_universe *universe, const size_t part_id)
{
  frc->x = 0.0;
  frc->y = 0.0;
  frc->z = 0.0;
}