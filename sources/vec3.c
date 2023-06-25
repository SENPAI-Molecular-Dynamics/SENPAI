/*
 * vec3.c
 *
 * Licensed under GPLv3 license
 *
 */

#include <math.h>
#include <stdlib.h>

#include "vec3.h"
#include "config.h"
#include "util.h"
#include "text.h"

/* dest = v1+v2 */
vec3_t *vec3_add(vec3_t *dest, const vec3_t *v1, const vec3_t *v2)
{
  dest->x = (v1->x + v2->x);
  dest->y = (v1->y + v2->y);
  dest->z = (v1->z + v2->z);

  return (dest);
}

/* dest = v1-v2 */
vec3_t *vec3_sub(vec3_t *dest, const vec3_t *v1, const vec3_t *v2)
{
  dest->x = (v1->x - v2->x);
  dest->y = (v1->y - v2->y);
  dest->z = (v1->z - v2->z);

  return (dest);
}

/* dest = v*lambda */
vec3_t *vec3_mul(vec3_t *dest, const vec3_t *v, const double lambda)
{
  dest->x = (v->x * lambda);
  dest->y = (v->y * lambda);
  dest->z = (v->z * lambda);

  return (dest);
}

/* dest = v/lambda */
vec3_t *vec3_div(vec3_t *dest, const vec3_t *v, const double lambda)
{
  if (lambda < DIV_THRESHOLD)
  {
    return (retstr(NULL, TEXT_VEC3_DIV_FAILURE, __FILE__, __LINE__));
  }

  dest->x = (v->x / lambda);
  dest->y = (v->y / lambda);
  dest->z = (v->z / lambda);
  return (dest);
}

/* dest = v1^v2 */
vec3_t *vec3_cross(vec3_t *dest, const vec3_t *v1, const vec3_t *v2)
{
  dest->x = (v1->y * v2->z) - (v1->z * v2->y);
  dest->y = (v1->z * v2->x) - (v1->x * v2->z);
  dest->z = (v1->x * v2->y) - (v1->y * v2->x);

  return (dest);
}

/* Sets dest as the unit vector derived from v */
vec3_t *vec3_unit(vec3_t *dest, const vec3_t *v)
{
  if (vec3_div(dest, v, vec3_mag(v)) == NULL)
  {
    return (retstr(NULL, TEXT_VEC3_UNIT_FAILURE, __FILE__, __LINE__));
  }

  return (dest);
}

/* Generate a random vector from the unit sphere*/
/* Method from Marsaglia, 1972 */
vec3_t *vec3_marsaglia(vec3_t *v)
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

/* Returns the dot product of the two provided vectors */
double vec3_dot(const vec3_t *v1, const vec3_t *v2)
{
   return ((v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z));
}

/* Returns the angle between the two provided vectors */
double vec3_ang(const vec3_t *v1, const vec3_t *v2)
{
  double dot;
  double len_v1;
  double len_v2;

  dot = vec3_dot(v1, v2);
  len_v1 = vec3_mag(v1);
  len_v2 = vec3_mag(v2);

  /* We don't need to check if the acos input is within the defined range */
  /* It always is */
  return (acos(dot/(len_v1*len_v2)));
}

/* Returns the magnitude of the provided vector */
double vec3_mag(const vec3_t *v)
{
  return (sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z)));
}





/* Apply a transformation matrix to a vector */
mat3_t *mat3_transform_apply(mat3_t *m, vec3_t *v)
{
  v->x = ((m->x0)*(v->x)) + ((m->y0)*(v->y)) + ((m->z0)*(v->z));
  v->y = ((m->x1)*(v->x)) + ((m->y1)*(v->y)) + ((m->z1)*(v->z));
  v->z = ((m->x2)*(v->x)) + ((m->y2)*(v->y)) + ((m->z2)*(v->z));

  return (m);
}

/* Generates a rotation transform matrix */
mat3_t *mat3_transform_gen_rot(mat3_t *m, vec3_t *axis, const double angle)
{
    double c = cos(angle);
    double s = sin(angle);
    double t = 1.0 - c;

    double x = axis->x;
    double y = axis->y;
    double z = axis->z;

    m->x0 = (x * x * t) + c;
    m->x1 = (x * y * t) - (z * s);
    m->x2 = (x * z * t) + (y * s);

    m->y0 = (y * x * t) + (z * s);
    m->y1 = (y * y * t) + c;
    m->y2 = (y * z * t) - (x * s);

    m->z0 = (z * x * t) - (y * s); 
    m->z1 = (z * y * t) + (x * s);
    m->z2 = (z * z * t) + c;

    return (m);
}
