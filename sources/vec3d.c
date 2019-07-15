/*
 * vec3d.c
 *
 * Licensed under MIT license
 *
 */

#include <math.h>
#include <stdlib.h>

#include "vec3d.h"
#include "util.h"
#include "text.h"

/* dest = v1+v2 */
vec3d_t *vec3d_add(vec3d_t *dest, const vec3d_t *v1, const vec3d_t *v2)
{
  dest->x = (v1->x + v2->x);
  dest->y = (v1->y + v2->y);
  dest->z = (v1->z + v2->z);

  return (dest);
}

/* dest = v1-v2 */
vec3d_t *vec3d_sub(vec3d_t *dest, const vec3d_t *v1, const vec3d_t *v2)
{
  dest->x = (v1->x - v2->x);
  dest->y = (v1->y - v2->y);
  dest->z = (v1->z - v2->z);

  return (dest);
}

/* dest = v*lambda */
vec3d_t *vec3d_mul(vec3d_t *dest, const vec3d_t *v, const double lambda)
{
  dest->x = (v->x * lambda);
  dest->y = (v->y * lambda);
  dest->z = (v->z * lambda);

  return (dest);
}

/* dest = v/lambda */
vec3d_t *vec3d_div(vec3d_t *dest, const vec3d_t *v, const double lambda)
{
  if (lambda < 10E-30)
    return (retstr(NULL, TEXT_VEC3D_DIV_FAILURE, __FILE__, __LINE__));

  dest->x = (v->x / lambda);
  dest->y = (v->y / lambda);
  dest->z = (v->z / lambda);
  return (dest);
}

/* Returns the dot product of the two provided vectors */
double vec3d_dot(const vec3d_t *v1, const vec3d_t *v2)
{
   return ((v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z));
}

/* Returns the magnitude of the provided vector */
double vec3d_mag(const vec3d_t *v)
{
  return (sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z)));
}

/* dest = v1^v2 */
vec3d_t *vec3d_cross(vec3d_t *dest, const vec3d_t *v1, const vec3d_t *v2)
{
  dest->x = (v1->y * v2->z) - (v1->z * v2->y);
  dest->y = (v1->z * v2->x) - (v1->x * v2->z);
  dest->z = (v1->x * v2->y) - (v1->y * v2->x);

  return (dest);
}

/* Sets dest as the unit vector derived from v */
vec3d_t *vec3d_unit(vec3d_t *dest, const vec3d_t *v)
{
  double mag;
  vec3d_t *res;

  mag = vec3d_mag(v);

  if ((res = vec3d_div(dest, v, mag)) == NULL)
    return (retstr(NULL, TEXT_VEC3D_UNIT_FAILURE, __FILE__, __LINE__));

  return (res);
}

/* Generate a random vector from the unit sphere*/
/* Method from Marsaglia, 1972 */
vec3d_t *vec3d_marsaglia(vec3d_t *v)
{
  double x1;
  double x2;

  do
  {
    x1 = cos(rand());
    x2 = cos(rand());
  } while ((x1*x1)+(x2*x2) > 1);

  v->x = 2*x1*sqrt(1-(x1*x1)-(x2*x2));
  v->y = 2*x2*sqrt(1-(x1*x1)-(x2*x2));
  v->z = 1-2*((x1*x1)+(x2*x2));

  return (v);
}