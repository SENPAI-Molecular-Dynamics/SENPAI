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
#include <lennard-jones.h>
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
  double r; /* Distance betwen the two particles */
  double sigma;
  double epsilon;
  double rsig; /* Reduced sigma (=sigma/r) */
  double rsig6; /* Above value raised to the sixth power */
  t_vec3d temp;

  /* Get the distance between the two particles */
  if (vec3d_sub(&temp, &(p1->pos), &(p2->pos)) == NULL)
    return (retstri(0, TEXT_CANTMATH, __FILE__, __LINE__));
  if ((r = vec3d_mag(&temp)) < 0.0)
    return (retstri(0, TEXT_CANTMATH, __FILE__, __LINE__));

  sigma = lj_sigma(p1, p2);
  epsilon = lj_epsilon(p1, p2);
  rsig = sigma/r;
  rsig6 = POW6(rsig);
  
  printf("r=%.50lf\n", r);
  return (-(24*epsilon/r) * ((2*rsig6*rsig6) - rsig6));
}
