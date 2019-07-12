/*
 * vec3d.h
 *
 * Licensed under MIT license
 *
 */

#ifndef VEC3D_H
#define VEC3D_H

typedef struct vec3d_s vec3d_t;
struct vec3d_s
{
  double x;
  double y;
  double z;
};

vec3d_t *vec3d_add(vec3d_t *dest, const vec3d_t *v1, const vec3d_t *v2);   /* dest = v1 + v2 */
vec3d_t *vec3d_sub(vec3d_t *dest, const vec3d_t *v1, const vec3d_t *v2);   /* dest = v1 - v2 */
vec3d_t *vec3d_mul(vec3d_t *dest, const vec3d_t *v, const double lambda);  /* dest = v * lambda */
vec3d_t *vec3d_div(vec3d_t *dest, const vec3d_t *v, const double lambda);  /* dest = v / lambda */
vec3d_t *vec3d_cross(vec3d_t *dest, const vec3d_t *v1, const vec3d_t *v2); /* dest = v1 ^ v2 */
vec3d_t *vec3d_unit(vec3d_t *dest, const vec3d_t *v);                      /* dest = v / |v| */

double vec3d_dot(const vec3d_t *v1, const vec3d_t *v2); /* Returns the dot product of v1 by v2 */
double vec3d_mag(const vec3d_t *v);                     /* Returns the vector's magnitude */

vec3d_t *vec3d_marsaglia(vec3d_t *v); /* Generates a random vector as per the 1972 Marsaglia method */

#endif
