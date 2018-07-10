/*
 * universe.c
 *
 * Licensed under MIT license
 *
 */

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
    fprintf(stderr, "[FAILURE} universe_init: Null argument (%s:%d)\n", __FILE__, __LINE__);
    return (NULL);
  }

  universe->c_grav = C_GRAV_DEFAULT;
  universe->c_elec = C_ELEC_DEFAULT;
  universe->c_time = C_TIME_DEFAULT;
  universe->c_size.x = C_SIZE_DEFAULT;
  universe->c_size.y = C_SIZE_DEFAULT;
  universe->c_size.z = C_SIZE_DEFAULT;  
  for (i=0; i<C_PART_NB; ++i)
  {
    if (particle_init(&(universe->particle[i])) == NULL)
    {
      fprintf(stderr, "[FAILURE] universe_init: can't initialize particle %ld (%s:%d)\n", i, __FILE__, __LINE__);
      return (NULL);
    }
  }
  return (&(universe->particle[0]));
}

t_particle *universe_updateparticle(t_universe *universe, const uint64_t part_id)
{
  t_vec3d temp;
  t_vec3d target; /* The vector to update */

  /* Update the position */
  target = universe->particle[part_id].pos;
  if (vec3d_mul(&temp, &(universe->particle[part_id].spd), universe->c_time) == NULL)
  {
    fprintf(stderr, "[FAILURE} universe_updateparticle: Couldn't do maths (%s:%d)\n", __FILE__, __LINE__);
    return (NULL);
  }
  if (vec3d_add(&(universe->particle[part_id].pos), &target, &temp) == NULL)
  { 
    fprintf(stderr, "[FAILURE} universe_updateparticle: Couldn't do maths (%s:%d)\n", __FILE__, __LINE__);
    return (NULL);
  }

  /* Update the speed */
  target = universe->particle[part_id].spd;
  if (vec3d_mul(&temp, &(universe->particle[part_id].acc), universe->c_time) == NULL)
  { 
    fprintf(stderr, "[FAILURE} universe_updateparticle: Couldn't do maths (%s:%d)\n", __FILE__, __LINE__);
    return (NULL);
  }
  if (vec3d_add(&(universe->particle[part_id].spd), &target, &temp) == NULL)
  {                                                                                      
    fprintf(stderr, "[FAILURE} universe_updateparticle: Couldn't do maths (%s:%d)\n", __FILE__, __LINE__);
    return (NULL);
  }

  /* Update the acceleration */
  if (vec3d_div(&(universe->particle[part_id].acc), &(universe->particle[part_id].frc), universe->particle[part_id].mass) == NULL)
  {                                                                                      
    fprintf(stderr, "[FAILURE} universe_updateparticle: Couldn't do maths (%s:%d)\n", __FILE__, __LINE__);
    return (NULL);
  }
  return (&(universe->particle[part_id]));
}
