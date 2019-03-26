/*
 * universe.c
 *
 * Licensed under MIT license
 *
 */

#include <text.h>
#include <stdio.h>
#include <vec3d.h>
#include <util.h>
#include <constants.h>
#include <universe.h>

t_universe *universe_init(t_universe *universe)
{
  size_t i;

  if (universe == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_NULLARG, __FILE__, __LINE__));

  universe->c_grav = C_GRAV_DEFAULT;
  universe->c_elec = C_ELEC_DEFAULT;
  universe->c_time = C_TIME_DEFAULT;
  universe->time = 0.0;
  for (i=0; i<C_PART_NB; ++i)
    if (particle_init(&(universe->particle[i])) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_INIT_NULLARG, __FILE__, __LINE__));

  return (universe);
}

t_universe *universe_iterate(t_universe *universe)
{
  uint64_t i;

  /* Holy shit this is so computationally expensive, thinking about OpenCL */
  for (i=0; i<C_PART_NB; ++i)
    particle_update_frc(universe, i);
  for (i=0; i<C_PART_NB; ++i)
    particle_update_acc(universe, i);
  for (i=0; i<C_PART_NB; ++i)
    particle_update_spd(universe, i);
  for (i=0; i<C_PART_NB; ++i)
    particle_update_pos(universe, i);

  return (universe);
}

t_particle *particle_init(t_particle *particle)
{
  particle->pos = e_0;
  particle->spd = e_0;
  particle->acc = e_0;
  particle->frc = e_0;

  particle->mass = 1.0;
  particle->charge = 1.0;
  particle->radius = 1.0;
  return (particle);
}


t_universe *particle_update_frc(t_universe *universe, const uint64_t part_id)
{
  t_particle *current;
  t_vec3d temp;
  uint64_t i;
  double dst;
  double frc_grav;
  double frc_elec;

  current = &(universe->particle[part_id]);
  for (i=0; i<C_PART_NB; ++i)
  {
    if (i != part_id)
    {
      /* Get the vector going to the target particle */
      if (vec3d_sub(&temp, &(universe->particle[i].pos), &(current->pos)) == NULL)
    	  return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
      /* Get its magnitude */
      if ((dst = vec3d_mag(&temp)) == 0.0)
    	  return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
      /* Turn it into its unit vector */
      if (vec3d_unit(&temp, &temp) == NULL)
    	  return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
      /* Compute the forces */
      frc_grav = (universe->c_grav)*(current->mass)*(universe->particle[i].mass)/(dst*dst);
      frc_elec = (universe->c_elec)*(current->charge)*(universe->particle[i].charge)/(dst*dst);
      /* Apply them to the particle */
      if (vec3d_mul(&(current->frc), &temp, frc_grav+frc_elec) == NULL)
    	  return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
    }
  }
  
  return (universe);
}

t_universe *particle_update_acc(t_universe *universe, const uint64_t part_id)
{
  t_particle *current;

  current = &(universe->particle[part_id]);
  if (vec3d_div(&(current->acc), &(current->frc), current->mass) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  return (universe);
}

t_universe *particle_update_spd(t_universe *universe, const uint64_t part_id)
{
  t_particle *current;
  t_vec3d temp;

  current = &(universe->particle[part_id]);
  if (vec3d_mul(&temp, &(current->acc), universe->c_time) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
  if (vec3d_add(&(current->spd), &(current->spd), &temp) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  return (universe);
}

t_universe *particle_update_pos(t_universe *universe, const uint64_t part_id)
{
  t_particle *current;
  t_vec3d temp;

  current = &(universe->particle[part_id]);
  if (vec3d_mul(&temp, &(current->spd), universe->c_time) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));
  if (vec3d_add(&(current->pos), &(current->pos), &temp) == NULL)
    return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

  return (universe);
}
