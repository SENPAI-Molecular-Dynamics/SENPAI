/*
 * particle.c
 *
 * Licensed under MIT license
 *
 */

#include <string.h>

#include "force.h"
#include "potential.h"
#include "universe.h"
#include "util.h"
#include "vec3d.h"

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
  /* Used in the numerical method */
  uint8_t err_flag;
  double potential;
  double h;

  /* Used in the analytical method */
  vec3d_t vec_callback;
  vec3d_t vec_bond;
  vec3d_t vec_electrostatic;
  vec3d_t vec_lennardjones;
  vec3d_t vec_torsion;

  /* If we use numerical solving */
  if (universe->force_computation_mode == MODE_NUMERICAL)
  {
    err_flag = 0;

    /* Differentiate potential over x axis */
    h = ROOT_MACHINE_EPSILON * (universe->particle[part_id].pos.x);
    universe->particle[part_id].pos.x -= h;
    potential = potential_total(universe, part_id, &err_flag);
    universe->particle[part_id].pos.x += 2*h;
    universe->particle[part_id].frc.x = -(potential_total(universe, part_id, &err_flag) - potential)/(2*h);
    universe->particle[part_id].pos.x -= h;
    
    /* Differentiate potential over y axis */
    h = ROOT_MACHINE_EPSILON * (universe->particle[part_id].pos.y);
    universe->particle[part_id].pos.y -= h;
    potential = potential_total(universe, part_id, &err_flag);
    universe->particle[part_id].pos.y += 2*h;
    universe->particle[part_id].frc.y = -(potential_total(universe, part_id, &err_flag) - potential)/(2*h);
    universe->particle[part_id].pos.y -= h;

    /* Differentiate potential over z axis */
    h = ROOT_MACHINE_EPSILON * (universe->particle[part_id].pos.z);
    universe->particle[part_id].pos.z -= h;
    potential = potential_total(universe, part_id, &err_flag);
    universe->particle[part_id].pos.z += 2*h;
    universe->particle[part_id].frc.z = -(potential_total(universe, part_id, &err_flag) - potential)/(2*h);
    universe->particle[part_id].pos.z -= h;
    
    /* We check if an error happened during potential energy computation */
    if (err_flag)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
  }

  /* If we use analytical solving */
  else
  {
    /* Reset the force vectors */
    universe->particle[part_id].frc.x = 0.0;
    universe->particle[part_id].frc.y = 0.0;
    universe->particle[part_id].frc.z = 0.0;
    vec_callback.x = 0.0;
    vec_callback.y = 0.0;
    vec_callback.z = 0.0;
    vec_bond.x = 0.0;
    vec_bond.y = 0.0;
    vec_bond.z = 0.0;
    vec_electrostatic.x = 0.0;
    vec_electrostatic.y = 0.0;
    vec_electrostatic.z = 0.0;
    vec_lennardjones.x = 0.0;
    vec_lennardjones.y = 0.0;
    vec_lennardjones.z = 0.0;
    vec_torsion.x = 0.0;
    vec_torsion.y = 0.0;
    vec_torsion.z = 0.0;

    /* Compute the forces */
    if (force_callback(&vec_callback, universe, part_id) == NULL)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
    if (force_bond(&vec_bond, universe, part_id) == NULL)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
    if (force_electrostatic(&vec_electrostatic, universe, part_id) == NULL)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
    if (force_lennardjones(&vec_lennardjones, universe, part_id) == NULL)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
    if (force_torsion(&vec_torsion, universe, part_id) == NULL)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));

    /* Apply them */
    if (vec3d_add(&(universe->particle[part_id].frc), &(universe->particle[part_id].frc), &vec_callback) == NULL)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
    if (vec3d_add(&(universe->particle[part_id].frc), &(universe->particle[part_id].frc), &vec_bond) == NULL)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
    if (vec3d_add(&(universe->particle[part_id].frc), &(universe->particle[part_id].frc), &vec_electrostatic) == NULL)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
    if (vec3d_add(&(universe->particle[part_id].frc), &(universe->particle[part_id].frc), &vec_lennardjones) == NULL)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
    if (vec3d_add(&(universe->particle[part_id].frc), &(universe->particle[part_id].frc), &vec_torsion) == NULL)
      return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));
  }

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
