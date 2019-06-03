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
  t_particle *current;
  t_vec3d temp;
  t_vec3d callback;
  t_vec3d frc_bond_vec[7]; /* Force vector for each bond */
  size_t i;
  size_t ii;
  double mag;
  double dst;
  double frc_elec;
  double frc_lj;

  current = &(universe->particle[part_id]);
  for (i=0; i<(universe->part_nb); ++i)
  {
    if (i != part_id)
    {
      /* Get the vector going to the target particle */
      if (vec3d_sub(&temp, &(universe->particle[i].pos), &(current->pos)) == NULL)
    	  return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

      /* Get its magnitude */
      if ((dst = vec3d_mag(&temp)) < 0.0)
    	  return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

      /* Turn it into its unit vector */
      if (vec3d_unit(&temp, &temp) == NULL)
    	  return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

      /* Compute the electrostatic and LJ forces */
      frc_elec = COUPLING_CNST_ELEC*(current->charge)*(universe->particle[i].charge)/(dst*dst);
      frc_lj = lennardjones(current, &(universe->particle[i]));

      /* Compute the bond forces */
      for (ii=0; ii<7; ++ii)
      {
        if ((current->bond[ii]) != NULL)
        {
          if (vec3d_sub(&(frc_bond_vec[ii]), &((current->bond[ii])->pos), &(current->pos)) == NULL)
            return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
          if (vec3d_unit(&(frc_bond_vec[ii]), &(frc_bond_vec[ii])) == NULL)
            return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
          if (vec3d_mul(&(frc_bond_vec[ii]), &(frc_bond_vec[ii]), bond_force(ii, current)) == NULL)
            return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
        }
        else
        {
          frc_bond_vec[ii].x = 0.0;
          frc_bond_vec[ii].y = 0.0;
          frc_bond_vec[ii].z = 0.0;
        }
      }
      
      /* Get the particle's distance from the origin */
      if ((mag = vec3d_mag(&(current->pos))) < 1E-50)
        return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
      
      /* Calculate the callback vector */
      if ((vec3d_mul(&callback, &(current->pos), -1E-5/mag) == NULL))
        return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
      
      /* Apply the forces */
      if (vec3d_mul(&(current->frc), &temp, frc_elec+frc_lj) == NULL)
    	  return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
      for (ii=0; ii<7; ++ii)
        if (vec3d_add(&(current->frc), &(current->frc), &(frc_bond_vec[ii])) == NULL)
          return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
      if ((vec3d_add(&(current->frc), &(current->frc), &callback)) == NULL)
        return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
    }
  }
  
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
  t_vec3d temp;

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
  t_vec3d temp;

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
