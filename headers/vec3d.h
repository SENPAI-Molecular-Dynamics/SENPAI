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

vec3d_t *vec3d_add(vec3d_t *dest, const vec3d_t *v1, const vec3d_t *v2);
vec3d_t *vec3d_sub(vec3d_t *dest, const vec3d_t *v1, const vec3d_t *v2);
vec3d_t *vec3d_mul(vec3d_t *dest, const vec3d_t *v, const double lambda);
vec3d_t *vec3d_div(vec3d_t *dest, const vec3d_t *v, const double lambda);

double vec3d_dot(const vec3d_t *v1, const vec3d_t *v2);
double vec3d_mag(const vec3d_t *v);

vec3d_t *vec3d_cross(vec3d_t *dest, const vec3d_t *v1, const vec3d_t *v2);
vec3d_t *vec3d_unit(vec3d_t *dest, const vec3d_t *v);

#endif
