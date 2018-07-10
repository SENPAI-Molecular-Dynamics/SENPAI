/*
 * particle.h
 *
 * Licensed under MIT license
 *
 */

#ifndef PARTICLE_H
#define PARTICLE_H

#include <vec3d.h>
#include <stdint.h>

typedef struct s_particle t_particle;
struct s_particle
{
  double mass;
  double charge;
  uint64_t id;

  t_vec3d pos;
  t_vec3d spd;
  t_vec3d acc;
  t_vec3d frc;
};

t_particle *particle_init(t_particle *particle);

#endif
