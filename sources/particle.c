/*
 * atom.c
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

/* Initialise a atom's memory */
atom_t *atom_init(atom_t *atom)
{
  atom->element = ATOM_NULL;
  atom->charge = 0.0;
  atom->epsilon = 0.0;
  atom->sigma = 0.0;

  atom->bond_nb = 7;

  atom->bond[0] = NULL;
  atom->bond[1] = NULL;
  atom->bond[2] = NULL;
  atom->bond[3] = NULL;
  atom->bond[4] = NULL;
  atom->bond[5] = NULL;
  atom->bond[6] = NULL;

  atom->bond_id[0] = -1;
  atom->bond_id[1] = -1;
  atom->bond_id[2] = -1;
  atom->bond_id[3] = -1;
  atom->bond_id[4] = -1;
  atom->bond_id[5] = -1;
  atom->bond_id[6] = -1;

  atom->bond_strength[0] = 0.0;
  atom->bond_strength[1] = 0.0;
  atom->bond_strength[2] = 0.0;
  atom->bond_strength[3] = 0.0;
  atom->bond_strength[4] = 0.0;
  atom->bond_strength[5] = 0.0;
  atom->bond_strength[6] = 0.0;

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

  return (atom);
}

/* Get the force through numerical differentiation */
universe_t *atom_update_frc_numerical(universe_t *universe, const uint64_t part_id)
{
  double potential;
  double potential_new;
  double h;

  /* Reset the force vector */
  universe->atom[part_id].frc.x = 0.0;
  universe->atom[part_id].frc.y = 0.0;
  universe->atom[part_id].frc.z = 0.0;

  /* Differentiate potential over x axis */
  h = ROOT_MACHINE_EPSILON * (universe->atom[part_id].pos.x);
  universe->atom[part_id].pos.x -= h;
  if (potential_total(&potential, universe, part_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  universe->atom[part_id].pos.x += 2*h;
  if (potential_total(&potential_new, universe, part_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  universe->atom[part_id].frc.x = -(potential_new - potential)/(2*h);
  universe->atom[part_id].pos.x -= h;
  
  /* Differentiate potential over y axis */
  h = ROOT_MACHINE_EPSILON * (universe->atom[part_id].pos.y);
  universe->atom[part_id].pos.y -= h;
  if (potential_total(&potential, universe, part_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  universe->atom[part_id].pos.y += 2*h;
  if (potential_total(&potential_new, universe, part_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  universe->atom[part_id].frc.y = -(potential_new - potential)/(2*h);
  universe->atom[part_id].pos.y -= h;

  /* Differentiate potential over z axis */
  h = ROOT_MACHINE_EPSILON * (universe->atom[part_id].pos.z);
  universe->atom[part_id].pos.z -= h;
  if (potential_total(&potential, universe, part_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  universe->atom[part_id].pos.z += 2*h;
  if (potential_total(&potential_new, universe, part_id) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  universe->atom[part_id].frc.z = -(potential_new - potential)/(2*h);
  universe->atom[part_id].pos.z -= h;

  return (universe);
}

/* Get the force through analytical solving */
universe_t *atom_update_frc_analytical(universe_t *universe, const uint64_t part_id)
{
  /* Reset the force vector */
  universe->atom[part_id].frc.x = 0.0;
  universe->atom[part_id].frc.y = 0.0;
  universe->atom[part_id].frc.z = 0.0;

  if (force_total(&(universe->atom[part_id].frc), universe, part_id) == NULL)
    return (retstr(NULL, TEXT_POTENTIAL_TOTAL_FAILURE, __FILE__, __LINE__));

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *atom_update_acc(universe_t *universe, const uint64_t part_id)
{
  if (vec3d_div(&(universe->atom[part_id].acc), &(universe->atom[part_id].frc), model_mass(universe->atom[part_id].element)) == NULL)
    return (retstr(NULL, TEXT_ATOM_UPDATE_ACC_FAILURE, __FILE__, __LINE__));

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *atom_update_vel(universe_t *universe, const args_t *args, const uint64_t part_id)
{
  vec3d_t new_vel;

  /*
   * new_vel = acc*dt*0.5
   * vel += new_vel
   */
   
  vec3d_mul(&new_vel, &(universe->atom[part_id].acc), 0.5 * args->timestep);
  vec3d_add(&(universe->atom[part_id].vel), &(universe->atom[part_id].vel), &new_vel);

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *atom_update_pos(universe_t *universe, const args_t *args, const uint64_t part_id)
{
  vec3d_t temp;

  /*
   * new_pos = acc*dt*0.5
   * new_pos += vel
   * new_pos *= dt
   * pos += new_pos
   */
   
  vec3d_mul(&temp, &(universe->atom[part_id].acc), args->timestep * 0.5);
  vec3d_add(&temp, &temp, &(universe->atom[part_id].vel));
  vec3d_mul(&temp, &temp, args->timestep);
  vec3d_add(&(universe->atom[part_id].pos), &(universe->atom[part_id].pos), &temp);

  return (universe);
}

/* Enforce the periodic boundary conditions by relocating the atom, if required */
universe_t *atom_enforce_pbc(universe_t *universe, const uint64_t part_id)
{
  if (universe->atom[part_id].pos.x >= 0.5*(universe->size))
    universe->atom[part_id].pos.x -= universe->size;
  else if (universe->atom[part_id].pos.x < -0.5*(universe->size))
    universe->atom[part_id].pos.x += universe->size;

  if (universe->atom[part_id].pos.y >= 0.5*(universe->size))
    universe->atom[part_id].pos.y -= universe->size;
  else if (universe->atom[part_id].pos.y < -0.5*(universe->size))
    universe->atom[part_id].pos.y += universe->size;

  if (universe->atom[part_id].pos.z >= 0.5*(universe->size))
    universe->atom[part_id].pos.z -= universe->size;
  else if (universe->atom[part_id].pos.z < -0.5*(universe->size))
    universe->atom[part_id].pos.z += universe->size;

  return (universe);
}

/* Returns 0 if a1 is not bonded to a2, returns 1 if it is bonded to a2 */
int atom_is_bonded(const atom_t *a1, const atom_t *a2)
{
  if (a1 != a2->bond[0] &&
      a1 != a2->bond[1] &&
      a1 != a2->bond[2] &&
      a1 != a2->bond[3] &&
      a1 != a2->bond[4] &&
      a1 != a2->bond[5] &&
      a1 != a2->bond[6] &&
      a2 != a1->bond[0] &&
      a2 != a1->bond[1] &&
      a2 != a1->bond[2] &&
      a2 != a1->bond[3] &&
      a2 != a1->bond[4] &&
      a2 != a1->bond[5] &&
      a2 != a1->bond[6])
    return (0);
  return (1);
}