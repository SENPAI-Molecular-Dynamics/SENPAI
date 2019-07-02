/*
 * force.c
 *
 * Licensed under MIT license
 *
 */

#include <math.h>
#include <stdio.h>

#include "util.h"
#include "force.h"
#include "universe.h"

universe_t *force_bond(vec3d_t *frc, universe_t *universe, const size_t p1, const size_t p2)
{
  int bond_id;
  double displacement;
  double dst;
  vec3d_t vec;

  frc->x = 0.0;
  frc->y = 0.0;
  frc->z = 0.0;

  /* Get the difference vector */
  if (vec3d_sub(&vec, &(universe->particle[p2].pos), &(universe->particle[p1].pos)) == NULL)
    return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

  /* Get its magnitude */
  if ((dst = vec3d_mag(&vec)) < 0.0)
    return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

  /* Find the bond id */
  for (bond_id=0; universe->particle[p1].bond[bond_id] != &(universe->particle[p2]); ++bond_id);

  /* Compute the force vector */
  displacement = dst - (universe->particle[p1].bond_length[bond_id]);
  if (vec3d_mul(frc, &vec, -(displacement*(universe->particle[p1].bond_strength[bond_id]))/dst) == NULL)
    return (retstr(NULL, TEXT_FORCE_BOND_FAILURE, __FILE__, __LINE__));

  return (universe);
} 

universe_t *force_electrostatic(vec3d_t *frc, universe_t *universe, const size_t p1, const size_t p2)
{
  double dst;
  vec3d_t vec;

  frc->x = 0.0;
  frc->y = 0.0;
  frc->z = 0.0;

  /* Get the difference vector */
  if (vec3d_sub(&vec, &(universe->particle[p2].pos), &(universe->particle[p1].pos)) == NULL)
    return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));

  /* Get its magnitude */
  if ((dst = vec3d_mag(&vec)) < 0.0)
    return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));

  /* Compute the force vector */
  if (vec3d_mul(frc, &vec, C_ELEC*(universe->particle[p1].charge)*(universe->particle[p2].charge)/(POW3(dst))) == NULL)
    return (retstr(NULL, TEXT_FORCE_ELECTROSTATIC_FAILURE, __FILE__, __LINE__));

  return (universe);
}

universe_t *force_lennardjones(vec3d_t *frc, universe_t *universe, const size_t p1, const size_t p2)
{
  double sigma;
  double epsilon;
  double dst;
  vec3d_t vec;

  frc->x = 0.0;
  frc->y = 0.0;
  frc->z = 0.0;

  /* Lennard-Jones parameters for helium */
  sigma = 256E-12;
  epsilon = 1.4110228E-22;

  /* Get the difference vector */
  if (vec3d_sub(&vec, &(universe->particle[p2].pos), &(universe->particle[p1].pos)) == NULL)
    return (retstr(NULL, TEXT_FORCE_LENNARDJONES_FAILURE, __FILE__, __LINE__));

  /* Get its magnitude */
  if ((dst = vec3d_mag(&vec)) < 0.0)
    return (retstr(NULL, TEXT_FORCE_LENNARDJONES_FAILURE, __FILE__, __LINE__));

  /* If the particle is not beyond the cutoff distance */
  if (dst < LENNARDJONES_CUTOFF*sigma)
  {
    /* Compute the force vector
    if (vec3d_mul(frc, &vec, -24*POW6(sigma)*epsilon*(POW6(dst)-2*POW6(sigma))/(dst*POW12(dst))) == NULL)
      return (retstr(NULL, TEXT_FORCE_LENNARDJONES_FAILURE, __FILE__, __LINE__)); */
    
    if (vec3d_mul(frc, &vec, ((-24*epsilon/dst) * (2*POW12(sigma/dst) - POW6(sigma/dst)))/dst) == NULL)
      return (retstr(NULL, TEXT_FORCE_LENNARDJONES_FAILURE, __FILE__, __LINE__));
  }

  return (universe);
}

universe_t *force_torsion(vec3d_t *frc, universe_t *universe, const size_t p1, const size_t p2)
{
  (void)universe;
  (void)p1;
  (void)p2;

  frc->x = 0.0;
  frc->y = 0.0;
  frc->z = 0.0;

  return (universe);
}

universe_t *force_total(vec3d_t *frc, universe_t *universe, const size_t part_id)
{
  size_t i;
  vec3d_t vec_bond;
  vec3d_t vec_electrostatic;
  vec3d_t vec_lennardjones;
  vec3d_t vec_torsion;

  /* For each particle */
  for (i=0; i<(universe->part_nb); ++i)
  {
    /* That isn't the same as the current one */
    if (i != part_id)
    {

      /* Bonded interractions */
      if (particle_is_bonded(&(universe->particle[part_id]), &(universe->particle[i])))
      {
        if (force_bond(&vec_bond, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
        if (force_torsion(&vec_torsion, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));

        /* Sum the forces */
        if (vec3d_add(frc, frc, &vec_bond) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
        if (vec3d_add(frc, frc, &vec_torsion) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
      }

      /* Non-bonded interractions */
      else
      {
        if (force_electrostatic(&vec_electrostatic, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
        if (force_lennardjones(&vec_lennardjones, universe, part_id, i) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));

        /* Sum the forces */
        if (vec3d_add(frc, frc, &vec_electrostatic) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
        if (vec3d_add(frc, frc, &vec_lennardjones) == NULL)
          return (retstr(NULL, TEXT_FORCE_TOTAL_FAILURE, __FILE__, __LINE__));
      }
    }
  }

  return (universe);
}