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
#include "bond.h"

t_particle *particle_init(t_particle *particle)
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

t_universe *particle_update_frc(t_universe *universe, const uint64_t part_id)
{
  double dst;
  vec3d_t frc_callback; /* Force applied to the particle as it tries to leave the universe */
  vec3d_t frc_bond;
  vec3d_t frc_elec;
  vec3d_t frc_lennardjones;
  vec3d_t frc_torsion;

  /* Zero out the vectors */
  universe->particle[part_id].frc.x = 0.0;
  universe->particle[part_id].frc.y = 0.0;
  universe->particle[part_id].frc.z = 0.0;

  /* Get the particle's distance from the origin to check if it's leaving the universe */
  if ((dst = vec3d_mag(&(universe->particle[part_id].pos))) < 0.0)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

  /* If the particle is leaving the universe */
  if (dst >(universe->size))
  {
    /* Compute the callback force */
    if (force_callback(&frc_callback, universe, part_id) == NULL)
      return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

    /* Apply the callback force */
    if (vec3d_add(&(universe->particle[part_id].frc), &(universe->particle[part_id].frc), &frc_callback) == NULL)
      return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  }

  /* COMPUTE THE APPLIED FORCES */

  /* Compute the bond forces */
  if (force_bond(&frc_elec, universe, part_id) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

  /* Compute the electrostatic force */
  if (force_electrostatic(&frc_elec, universe, part_id) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

  /* Compute the 12-6 LJ force */
  if (force_lennardjones(&frc_lennardjones, universe, part_id) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

  /* Compute the torsion force */
  if (force_torsion(&frc_elec, universe, part_id) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

  /* APPLY THE COMPUTED FORCES */

  /* Apply the bond force */
  if (vec3d_add(&(universe->particle[part_id].frc), &(universe->particle[part_id].frc), &frc_bond) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

  /* Apply the electrostatic force */
  if (vec3d_add(&(universe->particle[part_id].frc), &(universe->particle[part_id].frc), &frc_elec) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

  /* Apply the 12-6 LJ force */
  if (vec3d_add(&(universe->particle[part_id].frc), &(universe->particle[part_id].frc), &frc_lennardjones) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

  /* Apply the torsion force */
  if (vec3d_add(&(universe->particle[part_id].frc), &(universe->particle[part_id].frc), &frc_torsion) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

  return (universe);
}

/* Velocity-Verlet integrator */
t_universe *particle_update_acc(t_universe *universe, const uint64_t part_id)
{
  t_particle *current;

  current = &(universe->particle[part_id]);
  if (vec3d_div(&(current->acc), &(current->frc), current->mass) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  return (universe);
}

/* Velocity-Verlet integrator */
t_universe *particle_update_spd(t_universe *universe, const t_args *args, const uint64_t part_id)
{
  t_particle *current;
  vec3d_t temp;

  current = &(universe->particle[part_id]);

  /* new_spd = acc*dt*0.5 */
  if (vec3d_mul(&temp, &(current->acc), args->timestep * 0.5) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  /* spd += new_spd */
  if (vec3d_add(&(current->spd), &(current->spd), &temp) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  return (universe);
}

/* Velocity-Verlet integrator */
t_universe *particle_update_pos(t_universe *universe, const t_args *args, const uint64_t part_id)
{
  t_particle *current;
  vec3d_t temp;

  current = &(universe->particle[part_id]);

  /* new_pos = acc*dt*0.5 */
  if (vec3d_mul(&temp, &(current->acc), args->timestep * 0.5) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  /* new_pos += spd */
  if (vec3d_add(&temp, &temp, &(current->spd)) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  /* new_pos *= dt */
  if (vec3d_mul(&temp, &temp, args->timestep) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  /* pos += new_pos */
  if (vec3d_add(&(current->pos), &(current->pos), &temp) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  return (universe);
}
