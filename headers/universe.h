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
#include <text.h>

typedef struct s_universe t_universe;
struct s_universe
{
  /* Universe constants */
  double  c_grav;
  double  c_elec;
  double  c_time;

  double time; /* Current time */
  t_particle particle[C_PART_NB];
  FILE *fd_tab[C_PART_NB]; /* Each particle will its log written to a corresponding .csv file */
};

t_particle *universe_init(t_universe *universe);
void universe_clean(t_universe *universe);
t_particle *universe_updateparticle(t_universe *universe, const uint64_t part_id);

#endif
