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
  universe->time = 0.0;
  for (i=0; i<C_PART_NB; ++i)
  {
    if (particle_init(&(universe->particle[i]), i) == NULL)
    {
      fprintf(stderr, "[FAILURE] universe_init: can't initialize particle %ld (%s:%d)\n", i, __FILE__, __LINE__);
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
    fprintf(stderr, "[FAILURE} universe_updateparticle: Couldn't do maths (%s:%d)\n", __FILE__, __LINE__);
    return (NULL);
  }
  if (vec3d_add(&(current_part->pos), &target, &temp) == NULL)
  { 
    fprintf(stderr, "[FAILURE} universe_updateparticle: Couldn't do maths (%s:%d)\n", __FILE__, __LINE__);
    return (NULL);
  }

  /* Update the speed */
  target = current_part->spd;
  if (vec3d_mul(&temp, &(current_part->acc), universe->c_time) == NULL)
  { 
    fprintf(stderr, "[FAILURE} universe_updateparticle: Couldn't do maths (%s:%d)\n", __FILE__, __LINE__);
    return (NULL);
  }
  if (vec3d_add(&(current_part->spd), &target, &temp) == NULL)
  {                                                                                      
    fprintf(stderr, "[FAILURE} universe_updateparticle: Couldn't do maths (%s:%d)\n", __FILE__, __LINE__);
    return (NULL);
  }

  /* Update the acceleration */
  if (vec3d_div(&(current_part->acc), &(current_part->frc), universe->particle[part_id].mass) == NULL)
  {                                                                                      
    fprintf(stderr, "[FAILURE} universe_updateparticle: Couldn't do maths (%s:%d)\n", __FILE__, __LINE__);
    return (NULL);
  }
  fprintf(stdout, "Particle updated (id= %.4ld, t=%.10lf, m=%.10lf, F=(%.10lf,%.10lf,%.10lf), a=(%.10lf,%.10lf,%.10lf), v=(%.10lf,%.10lf,%.10lf), pos=(%.10lf,%.10lf,%.10lf)\n",
      part_id,
      universe->time,
      current_part->mass,
      current_part->frc.x, current_part->frc.y, current_part->frc.z,
      current_part->acc.x, current_part->acc.y, current_part->acc.z,
      current_part->spd.x, current_part->spd.y, current_part->spd.z,
      current_part->pos.x, current_part->pos.y, current_part->pos.z
  );

  /* Update the force */
  current_part->frc = e_0;
  for (i=0; i<C_PART_NB; ++i)
  {
    vec3d_sub(&temp, &(universe->particle[i].pos), &(current_part->pos));
    dst = vec3d_mag(&temp);
    vec3d_div(&temp, &temp, dst); /* Turn temp into its unit vector */
    vec3d_mul(&temp, &temp, universe->c_grav*(current_part->mass)*(universe->particle[i].mass)/(dst*dst));
    vec3d_add(&(current_part->frc), &(current_part->frc), &temp);
  }
  /* Coulomb force */

  return (&(universe->particle[part_id]));
}
