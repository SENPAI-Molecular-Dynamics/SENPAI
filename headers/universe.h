/*
 * universe.h
 *
 * Licensed under MIT license
 *
 */

#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <stdint.h>
#include <stdio.h>

#include "vec3d.h"
#include "text.h"
#include "args.h"

#define COUPLING_CNST_ELEC (double)(8.98755*10E9)

typedef struct s_particle t_particle;
struct s_particle
{
  char element[3]; /* The element in question */
  double mass;
  double charge;

  t_particle *bond[7]; /* Points to the bonded particles (max. 7) */
  int64_t bond_id[7]; /* ID of the particle located at bond[ID] */
  double bond_strength[7]; /* Strength of each bond (spring constant) */
  double bond_length[7]; /* Length of the bond */

  t_vec3d pos;
  t_vec3d spd;
  t_vec3d acc;
  t_vec3d frc;
};

typedef struct s_universe t_universe;
struct s_universe
{
  uint64_t part_nb;
  uint64_t mol_nb;
  uint64_t mol_size;
  uint64_t iterations;

  double size;
  double time;          /* Current time */
  double temperature;
  t_particle *particle;

  FILE *output_file_xyz;
  char *input_file_buffer; /* A memory copy of input_file */
};

t_particle *particle_init(t_particle *particle);

t_universe *particle_update_frc(t_universe *universe, const uint64_t part_id);
t_universe *particle_update_acc(t_universe *universe, const uint64_t part_id);
t_universe *particle_update_spd(t_universe *universe, const t_args *args, const uint64_t part_id);
t_universe *particle_update_pos(t_universe *universe, const t_args *args, uint64_t part_id);
t_universe *universe_populate(t_universe *universe);
t_universe *universe_setvelocity(t_universe *universe);
t_universe *universe_load(t_universe *universe);
t_universe *universe_printstate(t_universe *universe);
int         universe_simulate(t_universe *universe, const t_args *args);
t_universe *universe_init(t_universe *universe, const t_args *args);
void        universe_clean(t_universe *universe);
t_universe *universe_iterate(t_universe *universe, const t_args *args);

#endif
