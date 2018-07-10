/*
 * particle.c
 *
 * Licensed under MIT License
 *
 */

#include <vec3d.h>
#include <particle.h>

t_particle *particle_init(t_particle *particle)
{
  particle->pos.x = 0.0;
  particle->pos.y = 0.0;
  particle->pos.z = 0.0;

  particle->spd.x = 0.0;
  particle->spd.y = 0.0;
  particle->spd.z = 0.0;

  particle->acc.x = 0.0;
  particle->acc.y = 0.0;
  particle->acc.z = 0.0;

  particle->frc.x = 0.0;
  particle->frc.y = 0.0;
  particle->frc.z = 0.0;

  return (particle);
}
