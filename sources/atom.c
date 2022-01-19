/*
 * atom.c
 *
 * Licensed under GPLv3 license
 *
 */

#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "force.h"
#include "model.h"
#include "potential.h"
#include "universe.h"
#include "util.h"
#include "vec3.h"

/* Initialise an atom structure */
void atom_init(atom_t *atom)
{
  atom->element=ATOM_ELEMENT_DEFAULT;

  atom->charge=ATOM_CHARGE_DEFAULT;
  atom->epsilon=ATOM_EPSILON_DEFAULT;
  atom->sigma=ATOM_SIGMA_DEFAULT;

  atom->bond_nb=ATOM_BOND_NB_DEFAULT;
  atom->bond=ATOM_BOND_DEFAULT;
  atom->bond_strength=ATOM_BOND_STRENGTH_DEFAULT;

  atom->pos.x=ATOM_POS_X_DEFAULT;
  atom->pos.y=ATOM_POS_Y_DEFAULT;
  atom->pos.z=ATOM_POS_Z_DEFAULT;

  atom->vel.x=ATOM_VEL_X_DEFAULT;
  atom->vel.y=ATOM_VEL_Y_DEFAULT;
  atom->vel.z=ATOM_VEL_Z_DEFAULT;

  atom->acc.x=ATOM_ACC_X_DEFAULT;
  atom->acc.y=ATOM_ACC_Y_DEFAULT;
  atom->acc.z=ATOM_ACC_Z_DEFAULT;

  atom->frc.x=ATOM_FRC_X_DEFAULT;
  atom->frc.y=ATOM_FRC_Y_DEFAULT;
  atom->frc.z=ATOM_FRC_Z_DEFAULT;
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
  universe->atom[atom_id].frc.x = ATOM_FRC_X_DEFAULT;
  universe->atom[atom_id].frc.y = ATOM_FRC_Y_DEFAULT;
  universe->atom[atom_id].frc.z = ATOM_FRC_Z_DEFAULT;

  /* Differentiate potential over x axis */
  h = ROOT_MACHINE_EPSILON * (universe->atom[atom_id].pos.x);
  universe->atom[atom_id].pos.x -= h;
  
  if (potential_total(&potential, universe, atom_id) == NULL)
  {
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  }
  
  universe->atom[atom_id].pos.x += 2*h;
  
  if (potential_total(&potential_new, universe, atom_id) == NULL)
  {
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  }
  
  universe->atom[atom_id].frc.x = -(potential_new - potential)/(2*h);
  universe->atom[atom_id].pos.x -= h;



  /* Differentiate potential over y axis */
  h = ROOT_MACHINE_EPSILON * (universe->atom[atom_id].pos.y);
  universe->atom[atom_id].pos.y -= h;
  
  if (potential_total(&potential, universe, atom_id) == NULL)
  {
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  }
  
  universe->atom[atom_id].pos.y += 2*h;
  
  if (potential_total(&potential_new, universe, atom_id) == NULL)
  {
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  }
  
  universe->atom[atom_id].frc.y = -(potential_new - potential)/(2*h);
  universe->atom[atom_id].pos.y -= h;



  /* Differentiate potential over z axis */
  h = ROOT_MACHINE_EPSILON * (universe->atom[atom_id].pos.z);
  universe->atom[atom_id].pos.z -= h;
  
  if (potential_total(&potential, universe, atom_id) == NULL)
  {
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  }
  
  universe->atom[atom_id].pos.z += 2*h;
  
  if (potential_total(&potential_new, universe, atom_id) == NULL)
  {
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  }
  
  universe->atom[atom_id].frc.z = -(potential_new - potential)/(2*h);
  universe->atom[atom_id].pos.z -= h;

  return (universe);
}

/* Get the force through numerical differentiation in a tetrahedron */
universe_t *atom_update_frc_numerical_tetrahedron(universe_t *universe, const uint64_t atom_id)
{
  double potential_000;
  double potential_110;
  double potential_011;
  double potential_101;
  double hx;
  double hy;
  double hz;

  /* Reset the force vector */
  universe->atom[atom_id].frc.x = 0.0;
  universe->atom[atom_id].frc.y = 0.0;
  universe->atom[atom_id].frc.z = 0.0;

  /* I'm not entirely sure why these are multiplied by the position, but this is how the other numerical differentiation does h */
  hx = ROOT_MACHINE_EPSILON * (universe->atom[atom_id].pos.x);
  hy = ROOT_MACHINE_EPSILON * (universe->atom[atom_id].pos.y);
  hz = ROOT_MACHINE_EPSILON * (universe->atom[atom_id].pos.z);

  /* Calculate potentials for each of the corners of the tetrahedron */
  universe->atom[atom_id].pos.x -= hx;
  universe->atom[atom_id].pos.y -= hy;
  universe->atom[atom_id].pos.z -= hz;
  if (potential_total(&potential_000, universe, atom_id) == NULL)
  {
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  }

  universe->atom[atom_id].pos.x += 2*hx;
  universe->atom[atom_id].pos.y += 2*hy;
  if (potential_total(&potential_110, universe, atom_id) == NULL)
  {
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  }

  universe->atom[atom_id].pos.x -= 2*hx;
  universe->atom[atom_id].pos.z += 2*hz;
  if (potential_total(&potential_011, universe, atom_id) == NULL)
  {
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  }

  universe->atom[atom_id].pos.x += 2*hx;
  universe->atom[atom_id].pos.y -= 2*hy;
  if (potential_total(&potential_101, universe, atom_id) == NULL)
  {
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  }

  universe->atom[atom_id].frc.x = -(potential_110 + potential_101 - potential_000 - potential_011)/(4*hx);
  universe->atom[atom_id].frc.y = -(potential_110 + potential_011 - potential_000 - potential_101)/(4*hy);
  universe->atom[atom_id].frc.z = -(potential_101 + potential_011 - potential_000 - potential_110)/(4*hz);
  universe->atom[atom_id].pos.x -= hx;
  universe->atom[atom_id].pos.y += hy;
  universe->atom[atom_id].pos.z -= hz;

  return (universe);
}

/* Get the force through analytical solving */
universe_t *atom_update_frc_analytical(universe_t *universe, const uint64_t atom_id)
{
  /* Reset the force vector */
  universe->atom[atom_id].frc.x = ATOM_FRC_X_DEFAULT;
  universe->atom[atom_id].frc.y = ATOM_FRC_Y_DEFAULT;
  universe->atom[atom_id].frc.z = ATOM_FRC_Z_DEFAULT;

  if (force_total(&(universe->atom[atom_id].frc), universe, atom_id) == NULL)
  {
    return (retstr(NULL, TEXT_ATOM_UPDATE_FRC_FAILURE, __FILE__, __LINE__));
  }

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *atom_update_acc(universe_t *universe, const uint64_t atom_id)
{
  if (vec3_div(&(universe->atom[atom_id].acc), &(universe->atom[atom_id].frc), universe->model.entry[universe->atom[atom_id].element].mass) == NULL)
  {
    return (retstr(NULL, TEXT_ATOM_UPDATE_ACC_FAILURE, __FILE__, __LINE__));
  }

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *atom_update_vel(universe_t *universe, const args_t *args, const uint64_t atom_id)
{
  vec3_t new_vel;

  /*
   * new_vel = acc*dt*0.5
   * vel += new_vel
   */

  vec3_mul(&new_vel, &(universe->atom[atom_id].acc), 0.5 * args->timestep);
  vec3_add(&(universe->atom[atom_id].vel), &(universe->atom[atom_id].vel), &new_vel);

  return (universe);
}

/* Velocity-Verlet integrator */
universe_t *atom_update_pos(universe_t *universe, const args_t *args, const uint64_t atom_id)
{
  vec3_t temp;

  /*
   * new_pos = acc*dt*0.5
   * new_pos += vel
   * new_pos *= dt
   * pos += new_pos
   */

  vec3_mul(&temp, &(universe->atom[atom_id].acc), args->timestep * 0.5);
  vec3_add(&temp, &temp, &(universe->atom[atom_id].vel));
  vec3_mul(&temp, &temp, args->timestep);
  vec3_add(&(universe->atom[atom_id].pos), &(universe->atom[atom_id].pos), &temp);

  return (universe);
}

/* Enforce the periodic boundary conditions by relocating the atom, if required */
universe_t *atom_enforce_pbc(universe_t *universe, const uint64_t atom_id)
{
  while (universe->atom[atom_id].pos.x >= 0.5*(universe->size))
  {
    universe->atom[atom_id].pos.x -= universe->size;
  }

  while (universe->atom[atom_id].pos.x < -0.5*(universe->size))
  {
    universe->atom[atom_id].pos.x += universe->size;
  }

  while (universe->atom[atom_id].pos.y >= 0.5*(universe->size))
  {
    universe->atom[atom_id].pos.y -= universe->size;
  }

  while (universe->atom[atom_id].pos.y < -0.5*(universe->size))
  {
    universe->atom[atom_id].pos.y += universe->size;
  }

  while (universe->atom[atom_id].pos.z >= 0.5*(universe->size))
  {
    universe->atom[atom_id].pos.z -= universe->size;
  }

  while (universe->atom[atom_id].pos.z < -0.5*(universe->size))
  {
    universe->atom[atom_id].pos.z += universe->size;
  }

  return (universe);
}

/* Returns 0 if a1 is not bonded to a2, returns 1 if it is bonded to a2 */
int atom_is_bonded(universe_t *universe, const uint64_t a1, const uint64_t a2)
{
  atom_t *atom_1;
  size_t i;

  atom_1 = &(universe->atom[a1]);
  for (i=0; i<(atom_1->bond_nb); ++i)
  {
    if (atom_1->bond[i] == a2)
    {
      return (1);
    }
  }

  return (0);
}
