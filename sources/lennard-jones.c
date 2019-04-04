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

#define POW6(x) (x*x*x*x*x*x)

double lj_epsilon(const t_particle *p1, const t_particle *p2)
{
}

double lj_sigma(const t_particle *p1, const t_particle *p2)
{
}

double lennardjones(const t_particle *p1, const t_particle *p2)
{
  double r; /* Distance betwen the two particles */
  double rc; /* Cutoff distance. No interation is computed if the distance is any greater */
  double r6;
  double A;
  double B;
  double sigma;
  double sigma6;
  double epsilon;
  t_vec3d temp;

  /* Get the distance between the two particles */
  if (vec3d_sub(&temp, &(p1->pos), &(p2->pos)) == NULL)
    return (retstri(0, TEXT_CANTMATH, __FILE__, __LINE__));
  if ((r = vec3d_mag(&temp)) < 0.0)
    return (retstri(0, TEXT_CANTMATH, __FILE__, __LINE__));

  sigma = lj_sigma(p1, p2);
  rc = 2.5*sigma;

  if (r > rc)
    return (0.0); /* We don't compute interaction beyond that distance */

  epsilon = lj_epsilon(p1, p2);
  sigma6 = POW6(sigma);
  r6 = POW6(r);
  A = (4.0*epsilon*sigma6);
  B = A*sigma6;

  return ((A/(r6*r6))-(B/r6)-(0.0163*epsilon));
}
