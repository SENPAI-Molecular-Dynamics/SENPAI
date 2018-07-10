/*
 * vec3d.h
 *
 * Licensed under MIT license
 *
 */

#ifndef VEC3D_H
#define VEC3D_H

typedef struct s_vec3d t_vec3d;
struct s_vec3d
{
  double x;
  double y;
  double z;
};

t_vec3d *vec3d_add(t_vec3d *dest, const t_vec3d *v1, const t_vec3d *v2);
t_vec3d *vec3d_sub(t_vec3d *dest, const t_vec3d *v1, const t_vec3d *v2);
t_vec3d *vec3d_mul(t_vec3d *dest, const t_vec3d *v, const double lambda);
t_vec3d *vec3d_div(t_vec3d *dest, const t_vec3d *v, const double lambda);

double vec3d_dot(const t_vec3d *v1, const t_vec3d *v2);
double vec3d_mag(const t_vec3d *v);

t_vec3d *vec3d_cross(t_vec3d *dest, const t_vec3d *v1, const t_vec3d *v2);
t_vec3d *vec3d_unit(t_vec3d *dest, const t_vec3d *v);

t_vec3d e_x, ey, ez;
#endif
