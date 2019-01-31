/*
 * universe.c
 *
 * Licensed under MIT license
 *
 */

#include <text.h>
#include <stdio.h>
#include <vec3d.h>
#include <particle.h>
#include <universe.h>
#include <constants.h>

t_particle *universe_init(t_universe *universe)
{
  size_t i;

  if (universe == NULL)
  {                                                                                      
    fprintf(stderr, TEXT_UNIVERSE_INIT_NULLARG, __FILE__, __LINE__);
    return (NULL);
  }

  universe->c_grav = C_GRAV_DEFAULT;
  universe->c_elec = C_ELEC_DEFAULT;
  universe->c_time = C_TIME_DEFAULT;
  universe->c_size.x = C_SIZE_DEFAULT;
  universe->c_size.y = C_SIZE_DEFAULT;
  universe->c_size.z = C_SIZE_DEFAULT;  
  universe->time = 0.0;
  for (i=0; i<C_PART_NB; ++i)
  {
    if (particle_init(&(universe->particle[i]), i) == NULL)
    {
      fprintf(stderr, TEXT_UNIVERSE_INIT_CANTINITPART, i, __FILE__, __LINE__);
      return (NULL);
    }
  }
  return (&(universe->particle[0]));
}

t_particle *universe_updateparticle(t_universe *universe, const uint64_t part_id)
{
  t_particle *current_part;
  t_vec3d temp;
  t_vec3d target; /* The vector to update */
  uint64_t i;
  double dst;

  current_part = &(universe->particle[part_id]);

  /* Update the position */
  target = current_part->pos;
  if (vec3d_mul(&temp, &(current_part->spd), universe->c_time) == NULL)
  {
    fprintf(stderr, TEXT_UNIVERSE_UPDATEPART_CANTMATH, __FILE__, __LINE__);
    return (NULL);
  }
  if (vec3d_add(&(current_part->pos), &target, &temp) == NULL)
  { 
    fprintf(stderr, TEXT_UNIVERSE_UPDATEPART_CANTMATH, __FILE__, __LINE__);
    return (NULL);
  }

  /* Update the speed */
  target = current_part->spd;
  if (vec3d_mul(&temp, &(current_part->acc), universe->c_time) == NULL)
  { 
    fprintf(stderr, TEXT_UNIVERSE_UPDATEPART_CANTMATH, __FILE__, __LINE__);
    return (NULL);
  }
  if (vec3d_add(&(current_part->spd), &target, &temp) == NULL)
  {                                                                                      
    fprintf(stderr, TEXT_UNIVERSE_UPDATEPART_CANTMATH, __FILE__, __LINE__);
    return (NULL);
  }

  /* Update the acceleration */
  if (vec3d_div(&(current_part->acc), &(current_part->frc), universe->particle[part_id].mass) == NULL)
  {                                                                                      
    fprintf(stderr, TEXT_UNIVERSE_UPDATEPART_CANTMATH, __FILE__, __LINE__);
    return (NULL);
  }

  /* Update the force */
  current_part->frc = e_0;
  for (i=0; i<C_PART_NB; ++i)
  {
    if (i != part_id)
    {
      if (vec3d_sub(&temp, &(universe->particle[i].pos), &(current_part->pos)) == NULL)
      {
        fprintf(stderr, TEXT_UNIVERSE_UPDATEPART_CANTMATH, __FILE__, __LINE__);
        return (NULL);
      }
      dst = vec3d_mag(&temp);
      if (vec3d_mul(&temp, &temp, universe->c_grav*(current_part->mass)*(universe->particle[i].mass)/(dst*dst*dst)) == NULL)
      {
        fprintf(stderr, TEXT_UNIVERSE_UPDATEPART_CANTMATH, __FILE__, __LINE__);
        return (NULL);
      }
      if (vec3d_add(&(current_part->frc), &(current_part->frc), &temp) == NULL)
      {
        fprintf(stderr, TEXT_UNIVERSE_UPDATEPART_CANTMATH, __FILE__, __LINE__);
        return (NULL);
      }
    }
  }

  fprintf(stdout, TEXT_UNIVERSE_UPDATEPART_LOG,
      part_id,
      universe->time,
      current_part->mass,
      current_part->frc.x, current_part->frc.y, current_part->frc.z,
      current_part->acc.x, current_part->acc.y, current_part->acc.z,
      current_part->spd.x, current_part->spd.y, current_part->spd.z,
      current_part->pos.x, current_part->pos.y, current_part->pos.z
  );

  return (&(universe->particle[part_id]));
}
