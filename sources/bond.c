/*
 * lennard-jones.c
 *
 * Licensed under MIT license
 *
 */

#include <text.h>
#include <util.h>
#include <vec3d.h>
#include <universe.h>
#include <bond.h>
#include <stdio.h>

#define POW6(x) (x*x*x*x*x*x)

double lj_epsilon(const t_particle *p1, const t_particle *p2)
{
  (void)p1;
  (void)p2;
  /* return the value for helium */
  return (1.4110228E-22);
}

double lj_sigma(const t_particle *p1, const t_particle *p2)
{
  (void)p1;
  (void)p2;
  /* return the value for helium */
  return (256E-12);
}

double lennardjones(const t_particle *p1, const t_particle *p2)
{
  size_t i;
  double r; /* Distance betwen the two particles */
  double sigma;
  double epsilon;
  double rsig; /* Reduced sigma (=sigma/r) */
  double rsig6; /* Above value raised to the sixth power */
  t_vec3d temp;

  /* If the two particles are bonded, don't compute the VDW forces */
  for (i=0; i<7; ++i)
    if (p1->bond[i] == p2)
      return (0.0);

  /* Get the distance between the two particles */
  if (vec3d_sub(&temp, &(p1->pos), &(p2->pos)) == NULL)
    return (retstri(0, TEXT_CANTMATH, __FILE__, __LINE__));
  if ((r = vec3d_mag(&temp)) < 0.0)
    return (retstri(0, TEXT_CANTMATH, __FILE__, __LINE__));

  sigma = lj_sigma(p1, p2);

  if (r > 2.5*sigma)
    return (0.0); /* cutoff */

  epsilon = lj_epsilon(p1, p2);
  rsig = sigma/r;
  rsig6 = POW6(rsig);
  
  return (-(24*epsilon/r) * ((2*rsig6*rsig6) - rsig6));
}

double bond_force(const size_t bond_id, const t_particle *p)
{
  double r;
  double displacement;
  t_vec3d temp;

  /* If not bonded */
  if (p->bond[bond_id] == NULL)
    return (0.0);

  /* Get the distance between the two particles */
  if (vec3d_sub(&temp, &(p->pos), &(p->bond[bond_id]->pos)) == NULL)
    return (retstri(0, TEXT_CANTMATH, __FILE__, __LINE__));
  if ((r = vec3d_mag(&temp)) < 0.0)
    return (retstri(0, TEXT_CANTMATH, __FILE__, __LINE__));

  displacement = (p->bond_length[bond_id]) + r;
  return ((p->bond_strength[bond_id])*displacement);
}
