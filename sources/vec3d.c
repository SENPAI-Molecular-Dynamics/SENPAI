/*
 * vec3d.c
 *
 * Licensed under MIT license
 *
 */

#include <math.h>
#include <stdio.h>

#include "vec3d.h"
#include "util.h"
#include "text.h"

vec3d_t *vec3d_add(vec3d_t *dest, const vec3d_t *v1, const vec3d_t *v2)
{
  if (dest == NULL || v1 == NULL || v2 == NULL)
    return (retstr(NULL, TEXT_VEC3D_ADD_FAILURE, __FILE__, __LINE__));

  dest->x = (v1->x + v2->x);
  dest->y = (v1->y + v2->y);
  dest->z = (v1->z + v2->z);
  return (dest);
}

vec3d_t *vec3d_sub(vec3d_t *dest, const vec3d_t *v1, const vec3d_t *v2)
{
  if (dest == NULL || v1 == NULL || v2 == NULL)
    return (retstr(NULL, TEXT_VEC3D_SUB_FAILURE, __FILE__, __LINE__));

  dest->x = (v1->x - v2->x);
  dest->y = (v1->y - v2->y);
  dest->z = (v1->z - v2->z);
  return (dest);
}

vec3d_t *vec3d_mul(vec3d_t *dest, const vec3d_t *v, const double lambda)
{
  if (dest == NULL || v == NULL)
    return (retstr(NULL, TEXT_VEC3D_MUL_FAILURE, __FILE__, __LINE__));

  dest->x = (v->x * lambda);
  dest->y = (v->y * lambda);
  dest->z = (v->z * lambda);
  return (dest);
}

vec3d_t *vec3d_div(vec3d_t *dest, const vec3d_t *v, const double lambda)
{
  if (dest == NULL || v == NULL)
    return (retstr(NULL, TEXT_VEC3D_DIV_FAILURE, __FILE__, __LINE__));
  else if (lambda < 10E-50)
    return (retstr(NULL, TEXT_VEC3D_DIV_FAILURE, __FILE__, __LINE__));

  dest->x = (v->x / lambda);
  dest->y = (v->y / lambda);
  dest->z = (v->z / lambda);
  return (dest);
}

double vec3d_dot(const vec3d_t *v1, const vec3d_t *v2)
{
  if (v1 == NULL || v2 == NULL)
    return (retstri(-1.0, TEXT_VEC3D_DOT_FAILURE, __FILE__, __LINE__));

  return ((v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z));
}

double vec3d_mag(const vec3d_t *v)
{
  if (v == NULL)
    return (retstri(-1.0, TEXT_VEC3D_MAG_FAILURE, __FILE__, __LINE__));

  return (sqrt((v->x * v->x)+(v->y * v->y)+(v->z * v->z)));
}

vec3d_t *vec3d_cross(vec3d_t *dest, const vec3d_t *v1, const vec3d_t *v2)
{
  if (dest == NULL || v1 == NULL || v2 == NULL)
    return (retstr(NULL, TEXT_VEC3D_CROSS_FAILURE, __FILE__, __LINE__));

  dest->x = (v1->y * v2->z) - (v1->z * v2->y);
  dest->y = (v1->z * v2->x) - (v1->x * v2->z);
  dest->z = (v1->x * v2->y) - (v1->y * v2->x);
  return (dest);
}

vec3d_t *vec3d_unit(vec3d_t *dest, const vec3d_t *v)
{
  double mag;
  vec3d_t *res;

  if (dest == NULL || v == NULL)
    return (retstr(NULL, TEXT_VEC3D_UNIT_FAILURE, __FILE__, __LINE__));

  if ((mag = vec3d_mag(v)) < 0.0)
    return (retstr(NULL, TEXT_VEC3D_UNIT_FAILURE, __FILE__, __LINE__));
  if ((res = vec3d_div(dest, v, mag)) == NULL)
    return (retstr(NULL, TEXT_VEC3D_UNIT_FAILURE, __FILE__, __LINE__));

  if (res == NULL)
    return (retstr(NULL, TEXT_VEC3D_UNIT_FAILURE, __FILE__, __LINE__));

  return (res);
}
