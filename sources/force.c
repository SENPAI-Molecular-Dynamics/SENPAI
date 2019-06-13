/*
 * force.c
 *
 * Licensed under MIT license
 *
 */

#include <math.h>

#include "util.h"
#include "force.h"
#include "universe.h"

universe_t *force_callback(vec3d_t *frc, universe_t *universe, const size_t part_id)
{
  double dst;
  double displacement;

  /* Get the particle's distance from the origin */
  if ((dst = vec3d_mag(&(universe->particle[part_id].pos))) < 0.0)
    return (retstr(NULL, TEXT_FORCE_CALLBACK_FAILURE, __FILE__, __LINE__));

  /* If it is leaving the universe, apply a callback force */
  if (dst > (universe->size))
  {
    displacement = dst - (universe->size);
    if (vec3d_mul(frc, &(universe->particle[part_id].pos), -1E4*displacement/dst) == NULL)
      return (retstr(NULL, TEXT_FORCE_CALLBACK_FAILURE, __FILE__, __LINE__));
  }

  return (universe);
}

universe_t *force_bond(vec3d_t *frc, universe_t *universe, const size_t part_id)
{
  size_t i;
  double displacement;
  double dst;
  vec3d_t vec;

  /* For every particle, except the considered one */
  for (i=0; i<7; ++i)
  {
    if (universe->particle[part_id].bond[i] != NULL)
    {
      /* Get the difference vector */
      if (vec3d_sub(&vec, &(universe->particle[part_id].pos), &(universe->particle[i].pos)) == NULL)
        return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

      /* Get its magnitude */
      if ((dst = vec3d_mag(&vec)) < 0.0)
        return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

      /* Compute the force vector */
      displacement = dst - (universe->particle[i].bond_length[i]);
      if (vec3d_mul(frc, &vec, (displacement*(universe->particle[i].bond_strength[i]))/dst) == NULL)
        return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

      /* Sum it */
      if (vec3d_add(frc, frc, &vec) == NULL)
        return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));
    }
  }

  return (universe);
}

universe_t *force_electrostatic(vec3d_t *frc, universe_t *universe, const size_t part_id)
{
  size_t i;
  double dst;
  vec3d_t vec;

  /* For every particle, except the considered one */
  for (i=0; i<(universe->part_nb); ++i)
  {
    if (i != part_id)
    {
      /* Get the difference vector */
      if (vec3d_sub(&vec, &(universe->particle[part_id].pos), &(universe->particle[i].pos)) == NULL)
        return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));

      /* Get its magnitude */
      if ((dst = vec3d_mag(&vec)) < 0.0)
        return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));

      /* Compute the force vector */
      if (vec3d_mul(frc, &vec, C_ELEC*(universe->particle[part_id].charge)*(universe->particle[i].charge)/(dst*dst*dst)) == NULL)
        return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));

      /* Sum it */
      if (vec3d_add(frc, frc, &vec) == NULL)
        return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));
    }
  }

  return (universe);
}

universe_t *force_lennardjones(vec3d_t *frc, universe_t *universe, const size_t part_id)
{
  size_t i;
  double dst;
  double sigma;
  double epsilon;
  vec3d_t vec;

  /* Lennard-Jones parameters for helium */
  sigma = 256E-12;
  epsilon = 1.4110228E-22;

  /* For every particle, except invalid ones */
  for (i=0; i<(universe->part_nb); ++i)
  {
    if (i != part_id &&
        &(universe->particle[i]) != universe->particle[part_id].bond[0] &&
        &(universe->particle[i]) != universe->particle[part_id].bond[1] &&
        &(universe->particle[i]) != universe->particle[part_id].bond[2] &&
        &(universe->particle[i]) != universe->particle[part_id].bond[3] &&
        &(universe->particle[i]) != universe->particle[part_id].bond[4] &&
        &(universe->particle[i]) != universe->particle[part_id].bond[5] &&
        &(universe->particle[i]) != universe->particle[part_id].bond[6])
    {
      /* Get the difference vector */
      if (vec3d_sub(&vec, &(universe->particle[part_id].pos), &(universe->particle[i].pos)) == NULL)
        return (retstr(NULL, TEXT_FORCE_LENNARDJONES_FAILURE, __FILE__, __LINE__));

      /* Get its magnitude */
      if ((dst = vec3d_mag(&vec)) < 0.0)
        return (retstr(NULL, TEXT_FORCE_LENNARDJONES_FAILURE, __FILE__, __LINE__));

      /* If the particle is beyond the cutoff distance */
      if (dst > LENNARDJONES_CUTOFF*sigma)

      /* Compute the force vector */
      if (vec3d_mul(frc, &vec, -(24*epsilon/dst)*((2*POW12(sigma/dst)) - POW6(sigma/dst))/dst) == NULL)
        return (retstr(NULL, TEXT_FORCE_LENNARDJONES_FAILURE, __FILE__, __LINE__));

      /* Sum it */
      if (vec3d_add(frc, frc, &vec) == NULL)
        return (retstr(NULL, TEXT_FORCE_LENNARDJONES_FAILURE, __FILE__, __LINE__));
    }
  }

  return (universe);
}

universe_t *force_torsion(vec3d_t *frc, universe_t *universe, const size_t part_id)
{
  (void)frc;
  (void)universe;
  (void)part_id;

  return (universe);
}