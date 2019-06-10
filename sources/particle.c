/*
 * particle.c
 *
 * Licensed under MIT license
 *
 */

#include <string.h>

#include "universe.h"
#include "vec3d.h"
#include "util.h"
#include "potential.h"

particle_t *particle_init(particle_t *particle)
{
  particle->pos.x = 0.0;
  particle->pos.y = 0.0;
  particle->pos.z = 0.0;

  particle->spd.x = 0.0;
  particle->spd.y = 0.0;
  particle->spd.z = 0.0;

  particle->spd.x = 0.0;
  particle->acc.y = 0.0;
  particle->acc.z = 0.0;

  particle->frc.x = 0.0;
  particle->frc.y = 0.0;
  particle->frc.z = 0.0;

  strcpy(particle->element, "??");
  particle->mass = 1.0;
  particle->charge = 0.0;
  return (particle);
}

universe_t *particle_update_frc(universe_t *universe, const uint64_t part_id)
{
  double dst;
  double potential;
  uint8_t err_flag;

  /* Zero out the force vector */
  universe->particle[part_id].frc.x = 0.0;
  universe->particle[part_id].frc.y = 0.0;
  universe->particle[part_id].frc.z = 0.0;

  potential = 0.0;
  err_flag = 0;

  /* Get the particle's distance from the origin to check if it's leaving the universe */
  if ((dst = vec3d_mag(&(universe->particle[part_id].pos))) < 0.0)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

  /* Compute the potentials */
  potential += potential_bond(universe, part_id, &err_flag);
  potential += potential_electrostatic(universe, part_id, &err_flag);
  potential += potential_lennardjones(universe, part_id, &err_flag);
  potential += potential_torsion(universe, part_id, &err_flag);
  /* If the particle is leaving the universe, compute the callback */
  if (dst >(universe->size))
    potential += potential_callback(universe, part_id, &err_flag);

  if (err_flag)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *particle_update_acc(universe_t *universe, const uint64_t part_id)
{
  particle_t *current;

  current = &(universe->particle[part_id]);
  if (vec3d_div(&(current->acc), &(current->frc), current->mass) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_ACC_FAILURE, __FILE__, __LINE__));

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *particle_update_spd(universe_t *universe, const args_t *args, const uint64_t part_id)
{
  particle_t *current;
  vec3d_t temp;

  current = &(universe->particle[part_id]);

  /* new_spd = acc*dt*0.5 */
  if (vec3d_mul(&temp, &(current->acc), args->timestep * 0.5) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_SPD_FAILURE, __FILE__, __LINE__));

  /* spd += new_spd */
  if (vec3d_add(&(current->spd), &(current->spd), &temp) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_SPD_FAILURE, __FILE__, __LINE__));

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *particle_update_pos(universe_t *universe, const args_t *args, const uint64_t part_id)
{
  particle_t *current;
  vec3d_t temp;

  current = &(universe->particle[part_id]);

  /* new_pos = acc*dt*0.5 */
  if (vec3d_mul(&temp, &(current->acc), args->timestep * 0.5) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_POS_FAILURE, __FILE__, __LINE__));

  /* new_pos += spd */
  if (vec3d_add(&temp, &temp, &(current->spd)) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_POS_FAILURE, __FILE__, __LINE__));

  /* new_pos *= dt */
  if (vec3d_mul(&temp, &temp, args->timestep) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_POS_FAILURE, __FILE__, __LINE__));

  /* pos += new_pos */
  if (vec3d_add(&(current->pos), &(current->pos), &temp) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_POS_FAILURE, __FILE__, __LINE__));

  return (universe);
}
