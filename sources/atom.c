/*
 * atom.c
 *
 * Licensed under MIT license
 *
 */

#include <stdlib.h>
#include <string.h>

#include "force.h"
#include "model.h"
#include "potential.h"
#include "universe.h"
#include "util.h"
#include "vec3d.h"

/* Initialise a atom's memory */
void atom_init(atom_t *atom)
{
  atom->element = ATOM_NULL;
  atom->charge = 0.0;
  atom->epsilon = 0.0;
  atom->sigma = 0.0;

  atom->bond_nb = 0;
  atom->bond = NULL;
  atom->bond_strength = NULL;

  atom->pos.x = 0.0;
  atom->pos.y = 0.0;
  atom->pos.z = 0.0;

  atom->vel.x = 0.0;
  atom->vel.y = 0.0;
  atom->vel.z = 0.0;

  atom->vel.x = 0.0;
  atom->acc.y = 0.0;
  atom->acc.z = 0.0;

  atom->frc.x = 0.0;
  atom->frc.y = 0.0;
  atom->frc.z = 0.0;
}

/* Cleans an atom structure */
void atom_clean(atom_t *atom)
{
  free(atom->bond);
  free(atom->bond_strength);
}

/* Get the force through numerical differentiation */
universe_t *atom_update_frc_numerical(universe_t *universe, const uint64_t atom_id)
{
  double potential;
  double potential_new;
  double h;

  /* Reset the force vector */
  universe->atom[atom_id].frc.x = 0.0;
  universe->atom[atom_id].frc.y = 0.0;
  universe->atom[atom_id].frc.z = 0.0;

  /* Differentiate potential over x axis */
  h = ROOT_MACHINE_EPSILON * (universe->atom[atom_id].pos.x);
  universe->atom[atom_id].pos.x -= h;
  if (potential_total(&potential, universe, atom_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  universe->atom[atom_id].pos.x += 2*h;
  if (potential_total(&potential_new, universe, atom_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  universe->atom[atom_id].frc.x = -(potential_new - potential)/(2*h);
  universe->atom[atom_id].pos.x -= h;
  
  /* Differentiate potential over y axis */
  h = ROOT_MACHINE_EPSILON * (universe->atom[atom_id].pos.y);
  universe->atom[atom_id].pos.y -= h;
  if (potential_total(&potential, universe, atom_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  universe->atom[atom_id].pos.y += 2*h;
  if (potential_total(&potential_new, universe, atom_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  universe->atom[atom_id].frc.y = -(potential_new - potential)/(2*h);
  universe->atom[atom_id].pos.y -= h;

  /* Differentiate potential over z axis */
  h = ROOT_MACHINE_EPSILON * (universe->atom[atom_id].pos.z);
  universe->atom[atom_id].pos.z -= h;
  if (potential_total(&potential, universe, atom_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  universe->atom[atom_id].pos.z += 2*h;
  if (potential_total(&potential_new, universe, atom_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  universe->atom[atom_id].frc.z = -(potential_new - potential)/(2*h);
  universe->atom[atom_id].pos.z -= h;

  return (universe);
}

/* Get the force through analytical solving */
universe_t *atom_update_frc_analytical(universe_t *universe, const uint64_t atom_id)
{
  /* Reset the force vector */
  universe->atom[atom_id].frc.x = 0.0;
  universe->atom[atom_id].frc.y = 0.0;
  universe->atom[atom_id].frc.z = 0.0;

  if (force_total(&(universe->atom[atom_id].frc), universe, atom_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *atom_update_acc(universe_t *universe, const uint64_t atom_id)
{
  if (vec3d_div(&(universe->atom[atom_id].acc), &(universe->atom[atom_id].frc), model_mass(universe->atom[atom_id].element)) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_ACC_FAILURE, __FILE__, __LINE__));

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *atom_update_vel(universe_t *universe, const args_t *args, const uint64_t atom_id)
{
  vec3d_t new_vel;

  /*
   * new_vel = acc*dt*0.5
   * vel += new_vel
   */
   
  vec3d_mul(&new_vel, &(universe->atom[atom_id].acc), 0.5 * args->timestep);
  vec3d_add(&(universe->atom[atom_id].vel), &(universe->atom[atom_id].vel), &new_vel);

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *atom_update_pos(universe_t *universe, const args_t *args, const uint64_t atom_id)
{
  vec3d_t temp;

  /*
   * new_pos = acc*dt*0.5
   * new_pos += vel
   * new_pos *= dt
   * pos += new_pos
   */
   
  vec3d_mul(&temp, &(universe->atom[atom_id].acc), args->timestep * 0.5);
  vec3d_add(&temp, &temp, &(universe->atom[atom_id].vel));
  vec3d_mul(&temp, &temp, args->timestep);
  vec3d_add(&(universe->atom[atom_id].pos), &(universe->atom[atom_id].pos), &temp);

  return (universe);
}

/* Enforce the periodic boundary conditions by relocating the atom, if required */
universe_t *atom_enforce_pbc(universe_t *universe, const uint64_t atom_id)
{
  while (universe->atom[atom_id].pos.x >= 0.5*(universe->size))
    universe->atom[atom_id].pos.x -= universe->size;
  while (universe->atom[atom_id].pos.x < -0.5*(universe->size))
    universe->atom[atom_id].pos.x += universe->size;

  while (universe->atom[atom_id].pos.y >= 0.5*(universe->size))
    universe->atom[atom_id].pos.y -= universe->size;
  while (universe->atom[atom_id].pos.y < -0.5*(universe->size))
    universe->atom[atom_id].pos.y += universe->size;

  while (universe->atom[atom_id].pos.z >= 0.5*(universe->size))
    universe->atom[atom_id].pos.z -= universe->size;
  while (universe->atom[atom_id].pos.z < -0.5*(universe->size))
    universe->atom[atom_id].pos.z += universe->size;

  return (universe);
}

/* Returns 0 if a1 is not bonded to a2, returns 1 if it is bonded to a2 */
int atom_is_bonded(universe_t *universe, const uint8_t a1, const uint8_t a2)
{
  atom_t *atom_1;
  size_t i;

  atom_1 = &(universe->atom[a1]);
  for (i=0; i<(atom_1->bond_nb); ++i)
    if (atom_1->bond[i] == a2)
      return (1);
  return (0);
}