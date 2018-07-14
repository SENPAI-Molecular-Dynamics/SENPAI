/*
 * universe.h
 *
 * Licensed under MIT license
 *
 */

#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <particle.h>
#include <constants.h>

typedef struct s_universe t_universe;
struct s_universe
{
  /* Universe constants */
  double  c_grav;
  double  c_elec;
  double  c_time;
  t_vec3d c_size; /* Size of the universe */

  t_particle particle[C_PART_NB];
};

t_particle *universe_init(t_universe *universe);
t_particle *universe_updateparticle(t_universe *universe, const uint64_t part_id);

#endif
