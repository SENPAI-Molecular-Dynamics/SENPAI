/*
 * particle.c
 *
 * Licensed under MIT license
 *
 */

#include <string.h>

#include "force.h"
#include "model.h"
#include "potential.h"
#include "universe.h"
#include "util.h"
#include "vec3d.h"

particle_t *particle_init(particle_t *particle)
{
  particle->element = ATOM_NULL;
  particle->bond_nb = 0;

  particle->charge = 0.0;
  particle->epsilon = 0.0;
  particle->sigma = 0.0;

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

  return (particle);
}

/* Update a particle's force */
universe_t *particle_update_frc(universe_t *universe, const uint64_t part_id)
{
  /* Used in the numerical method */
  uint8_t err_flag;
  double potential;
  double potential_new;
  double h;

  err_flag = 0;

  /* Reset the force vector */
  universe->particle[part_id].frc.x = 0.0;
  universe->particle[part_id].frc.y = 0.0;
  universe->particle[part_id].frc.z = 0.0;

  /* If we use numerical solving */
  if (universe->force_computation_mode == MODE_NUMERICAL)
  {

    /* Differentiate potential over x axis */
    h = ROOT_MACHINE_EPSILON * (universe->particle[part_id].pos.x);
    universe->particle[part_id].pos.x -= h;
    if (potential_total(&potential, universe, part_id) == NULL)
      return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
    universe->particle[part_id].pos.x += 2*h;
    if (potential_total(&potential_new, universe, part_id) == NULL)
      return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
    universe->particle[part_id].frc.x = -(potential_new - potential)/(2*h);
    universe->particle[part_id].pos.x -= h;
    
    /* Differentiate potential over y axis */
    h = ROOT_MACHINE_EPSILON * (universe->particle[part_id].pos.y);
    universe->particle[part_id].pos.y -= h;
    if (potential_total(&potential, universe, part_id) == NULL)
      return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
    universe->particle[part_id].pos.y += 2*h;
    if (potential_total(&potential_new, universe, part_id) == NULL)
      return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
    universe->particle[part_id].frc.y = -(potential_new - potential)/(2*h);
    universe->particle[part_id].pos.y -= h;

    /* Differentiate potential over z axis */
    h = ROOT_MACHINE_EPSILON * (universe->particle[part_id].pos.z);
    universe->particle[part_id].pos.z -= h;
    if (potential_total(&potential, universe, part_id) == NULL)
      return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
    universe->particle[part_id].pos.z += 2*h;
    if (potential_total(&potential_new, universe, part_id) == NULL)
      return (retstr(NULL, TEXT_PARTICLE_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
    universe->particle[part_id].frc.z = -(potential_new - potential)/(2*h);
    universe->particle[part_id].pos.z -= h;
    
    /* We check if an error happened during potential energy computation */
    if (err_flag)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
  }

  /* If we use analytical solving, compute and apply the force */
  else if (force_total(&(universe->particle[part_id].frc), universe, part_id) == NULL)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *particle_update_acc(universe_t *universe, const uint64_t part_id)
{
  if (vec3d_div(&(universe->particle[part_id].acc), &(universe->particle[part_id].frc), model_mass(universe->particle[part_id].element)) == NULL)
    return (retstr(NULL, TEXT_PARTICLE_UPDATE_ACC_FAILURE, __FILE__, __LINE__));

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *particle_update_spd(universe_t *universe, const args_t *args, const uint64_t part_id)
{
  vec3d_t new_spd;

  /*
   * new_spd = acc*dt*0.5
   * spd += new_spd
   */
   
  vec3d_mul(&new_spd, &(universe->particle[part_id].acc), 0.5 * args->timestep);
  vec3d_add(&(universe->particle[part_id].spd), &(universe->particle[part_id].spd), &new_spd);

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *particle_update_pos(universe_t *universe, const args_t *args, const uint64_t part_id)
{
  vec3d_t temp;

  /*
   * new_pos = acc*dt*0.5
   * new_pos += spd
   * new_pos *= dt
   * pos += new_pos
   */
   
  vec3d_mul(&temp, &(universe->particle[part_id].acc), args->timestep * 0.5);
  vec3d_add(&temp, &temp, &(universe->particle[part_id].spd));
  vec3d_mul(&temp, &temp, args->timestep);
  vec3d_add(&(universe->particle[part_id].pos), &(universe->particle[part_id].pos), &temp);

  return (universe);
}

/* Enforce the periodic boundary conditions by relocating the particle, if required */
universe_t *particle_enforce_pbc(universe_t *universe, const uint64_t part_id)
{
  if (universe->particle[part_id].pos.x >= 0.5*(universe->size))
    universe->particle[part_id].pos.x -= universe->size;
  else if (universe->particle[part_id].pos.x < -0.5*(universe->size))
    universe->particle[part_id].pos.x += universe->size;

  if (universe->particle[part_id].pos.y >= 0.5*(universe->size))
    universe->particle[part_id].pos.y -= universe->size;
  else if (universe->particle[part_id].pos.y < -0.5*(universe->size))
    universe->particle[part_id].pos.y += universe->size;

  if (universe->particle[part_id].pos.z >= 0.5*(universe->size))
    universe->particle[part_id].pos.z -= universe->size;
  else if (universe->particle[part_id].pos.z < -0.5*(universe->size))
    universe->particle[part_id].pos.z += universe->size;

  return (universe);
}

/* Returns 0 if p1 is not bonded to p2, returns 1 if it is bonded to p2 */
int particle_is_bonded(const particle_t *p1, const particle_t *p2)
{
  if (p1 != p2->bond[0] &&
      p1 != p2->bond[1] &&
      p1 != p2->bond[2] &&
      p1 != p2->bond[3] &&
      p1 != p2->bond[4] &&
      p1 != p2->bond[5] &&
      p1 != p2->bond[6] &&
      p2 != p1->bond[0] &&
      p2 != p1->bond[1] &&
      p2 != p1->bond[2] &&
      p2 != p1->bond[3] &&
      p2 != p1->bond[4] &&
      p2 != p1->bond[5] &&
      p2 != p1->bond[6])
    return (0);
  return (1);
}