/*
 * vec3.h
 *
 * Licensed under GPLv3 license
 *
 */

#ifndef VEC3_H
#define VEC3_H

/* Represents a 3D vector
 *
 * +- -+
 * | x |
 * | y |
 * | z |
 * +- -+
 *
 */
typedef struct vec3_s vec3_t;
struct vec3_s
{
  double x;
  double y;
  double z;
};

/* Reprensents a 3x3 matrix
 *
 * +-        -+
 * | x0 y0 z0 |
 * | x1 y1 z1 |
 * | x2 y2 z2 |
 * +-        -+
 *
 */
typedef struct mat3_s mat3_t;
struct mat3_s
{
  double x0;
  double x1;
  double x2;

  double y0;
  double y1;
  double y2;

  double z0;
  double z1;
  double z2;
};

vec3_t *vec3_add(vec3_t *dest, const vec3_t *v1, const vec3_t *v2);   /* dest = v1 + v2 */
vec3_t *vec3_sub(vec3_t *dest, const vec3_t *v1, const vec3_t *v2);   /* dest = v1 - v2 */
vec3_t *vec3_mul(vec3_t *dest, const vec3_t *v, const double lambda); /* dest = v * lambda */
vec3_t *vec3_div(vec3_t *dest, const vec3_t *v, const double lambda); /* dest = v / lambda */
vec3_t *vec3_cross(vec3_t *dest, const vec3_t *v1, const vec3_t *v2); /* dest = v1 ^ v2 */
vec3_t *vec3_unit(vec3_t *dest, const vec3_t *v);                     /* dest = v / |v| */
vec3_t *vec3_marsaglia(vec3_t *v); /* Generates a random vector as per the 1972 Marsaglia method */

double vec3_dot(const vec3_t *v1, const vec3_t *v2); /* Returns the dot product of v1 by v2 */
double vec3_ang(const vec3_t *v1, const vec3_t *v2); /* Returns the angle between v1 and v2 */
double vec3_mag(const vec3_t *v);                    /* Returns the vector's magnitude */

mat3_t *mat3_transform_apply(mat3_t *m, vec3_t *v); /* Apply a transform matrix to a vector */
mat3_t *mat3_transform_gen_rot(mat3_t *m, vec3_t *axis, const double angle); /* Generates a random rotation transform matrix */

#endif
