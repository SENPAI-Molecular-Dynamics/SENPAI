/*
 * lennard-jones.c
 *
 * Licensed under MIT license
 *
 */

#include <text.h.>
#include <util.h>
#include <vec3d.h>
#include <universe.h>
#include <lennard-jones.h>

double lj_epsilon(const t_particle *p1, const t_particle *p2)
{
}

double lj_sigma(const t_particle *p1, const t_particle *p2)
{
}

double lennardjones(const t_particle *p1, const t_particle *p2)
{
  /* Uses the AB form to compute the potential */
  double A;
  double B;
  double epsilon;
  double sigma;
  double sigma6;
  double sigma12;
  double r;
  double r6;
  double r12;
  t_vec3d temp;

  /* Get the distance between the two particles */
  if (vec3d_sub(&temp, &(p1->pos), &(p2->pos)) == NULL)
    return (restri(0, ERR_CANTMATH, __FILE__, __LINE__));
  r = vec3d_mag(&temp);
  r6 = r*r*r*r*r*r;
  r12 = r6*r6;

  /* Get the sigma values */
  sigma = lj_sigma(p1, p2);
  sigma6 = sigma*sigma*sigma*sigma*sigma*sigma;
  sigma12 = sigma6*sigma6;

  A = (4.0*epsilon*sigma12);
  B = (4.0*epsilon*sigma6);
  return ((A/r12)-(B/r6));
}
