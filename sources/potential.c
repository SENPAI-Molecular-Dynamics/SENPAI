/*
 * potential.c
 *
 * Licensed under MIT license
 *
 */

#include <stdint.h>

#include "text.h"
#include "util.h"
#include "vec3d.h"
#include "universe.h"
#include "potential.h"

/* Universe boundaries reaction */
double potential_callback(universe_t *universe, const size_t part_id, uint8_t *err_flag)
{
  double dst;

  /* Get the distance from the origin */
  if ((dst = vec3d_mag(&(universe->particle[part_id].pos))) < 0.0)
  {
    *err_flag = 1;
    return (retstrf(0.0, TEXT_POTENTIAL_CALLBACK_FAILURE, __FILE__, __LINE__));
  }

  /* Return the potential U=0.5*k*x^2 */
  if (dst > (universe->size))
    return (0.5*1E4*((dst - universe->size)/(universe->size))*((dst - universe->size)/(universe->size)));
  return (0.0);
}

/* Covalent bonds */
double potential_bond(universe_t *universe, const size_t part_id, uint8_t *err_flag)
{
  size_t i;
  double dst_mag;
  double displacement;
  double potential;
  vec3d_t dst;

  potential = 0.0;

  for (i=0; i<7; ++i)
  {
    if (universe->particle[part_id].bond[i] != NULL)
    {
      /* Get the position difference vector */
      if (vec3d_sub(&dst, &(universe->particle[part_id].pos), &(universe->particle[i].pos)) == NULL)
      {
        *err_flag = 1;
        return (retstrf(0.0, TEXT_POTENTIAL_BOND_FAILURE, __FILE__, __LINE__));
      }

      /* Get the distance between the two particles */
      if ((dst_mag = vec3d_mag(&dst)) < 0.0)
      {
        *err_flag = 1;
        return (retstrf(0.0, TEXT_POTENTIAL_BOND_FAILURE, __FILE__, __LINE__));
      }

      /* Compute the bond potential U=0.5*k*x^2 */
      displacement = (dst_mag - universe->particle[part_id].bond_length[i])/dst_mag;
      potential += 0.5*(universe->particle[part_id].bond_strength[i])*displacement*displacement;
    }
  }
  
  return (potential);
}

/* Electrostatic interaction */
double potential_electrostatic(universe_t *universe, const size_t part_id, uint8_t *err_flag)
{
  size_t i;
  double potential;
  double dst_mag;
  vec3d_t dst;

  potential = 0.0;

  for (i=0; i<(universe->part_nb); ++i)
  {
    if (i != part_id)
    {
      /* Get the position difference vector */
      if (vec3d_sub(&dst, &(universe->particle[part_id].pos), &(universe->particle[i].pos)) == NULL)
      {
        *err_flag = 1;
        return (retstrf(0.0, TEXT_POTENTIAL_ELEC_FAILURE, __FILE__, __LINE__));
      }

      /* Get the distance between the two particles */
      if ((dst_mag = vec3d_mag(&dst)) < 0.0)
      {
        *err_flag = 1;
        return (retstrf(0.0, TEXT_POTENTIAL_ELEC_FAILURE, __FILE__, __LINE__));
      }

      /* Compute the potential */
      potential += C_ELEC*(universe->particle[part_id].charge)*(universe->particle[i].charge)/(dst_mag*dst_mag*dst_mag);
    }
  }

  return (potential);
}

/* Van der Waals interaction */
double potential_lennardjones(universe_t *universe, const size_t part_id, uint8_t *err_flag)
{
  size_t i;
  double epsilon;
  double sigma;
  double potential;
  double dst_mag;
  vec3d_t dst;

  potential = 0.0;

  /* Values for helium */
  sigma = 256E-12;
  epsilon = 1.4110228E-22;

  for (i=0; i<(universe->part_nb); ++i)
  {
    if (i != part_id)
    {
      /* Get the position difference vector */
      if (vec3d_sub(&dst, &(universe->particle[part_id].pos), &(universe->particle[i].pos)) == NULL)
      {
        *err_flag = 1;
        return (retstrf(0.0, TEXT_POTENTIAL_LJ_FAILURE, __FILE__, __LINE__));
      }

      /* Get the distance between the two particles */
      if ((dst_mag = vec3d_mag(&dst)) < 0.0)
      {
        *err_flag = 1;
        return (retstrf(0.0, TEXT_POTENTIAL_LJ_FAILURE, __FILE__, __LINE__));
      }
      
      /* Compute the potential if required */
      if (dst_mag < 2.5*sigma)
        potential += 4*epsilon*((POW12(sigma)/POW12(dst_mag))-(POW6(sigma)/POW6(dst_mag)));
    }
  }

  return (potential);
}

/* Bond torsion interaction */
double potential_torsion(universe_t *universe, const size_t part_id, uint8_t *err_flag)
{
  (void)universe;
  (void)part_id;
  (void)err_flag;
  return (0.0);
}

/* Total potential energy of the particle */
double potential_total(universe_t *universe, const size_t part_id, uint8_t *err_flag)
{
  double potential;
  uint8_t err_flag_local;
  
  potential = 0.0;
  err_flag_local = 0;

  /* Compute the potentials */
  potential += potential_callback(universe, part_id, &err_flag_local);
  potential += potential_bond(universe, part_id, &err_flag_local);
  potential += potential_electrostatic(universe, part_id, &err_flag_local);
  potential += potential_lennardjones(universe, part_id, &err_flag_local);
  potential += potential_torsion(universe, part_id, &err_flag_local);

  /* We check if an error happened during potential energy computation */
  if (err_flag_local)
  {
    *err_flag = 1;
    return (retstrf(0.0, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
  }
  return (potential);
}