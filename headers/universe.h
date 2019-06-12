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

typedef struct particle_s particle_t;
struct particle_s
{
  char element[3]; /* The element in question */
  double mass;
  double charge;

  particle_t *bond[7]; /* Points to the bonded particles (max. 7) */
  int64_t bond_id[7]; /* ID of the particle located at bond[ID] */
  double bond_strength[7]; /* Strength of each bond (spring constant) */
  double bond_length[7]; /* Length of the bond */

  vec3d_t pos;
  vec3d_t spd;
  vec3d_t acc;
  vec3d_t frc;
};

typedef struct universe_s universe_t;
struct universe_s
{
  uint64_t part_nb;
  uint64_t mol_nb;
  uint64_t mol_size;
  uint64_t iterations;

  double size;
  double time;          /* Current time */
  double temperature;
  particle_t *particle;

  uint8_t force_computation_mode;
  FILE *output_file_xyz;
  char *input_file_buffer; /* A memory copy of input_file */
};

particle_t *particle_init(particle_t *particle);

universe_t *particle_update_frc(universe_t *universe, const uint64_t part_id);
universe_t *particle_update_acc(universe_t *universe, const uint64_t part_id);
universe_t *particle_update_spd(universe_t *universe, const args_t *args, const uint64_t part_id);
universe_t *particle_update_pos(universe_t *universe, const args_t *args, uint64_t part_id);
universe_t *universe_populate(universe_t *universe);
universe_t *universe_setvelocity(universe_t *universe);
universe_t *universe_load(universe_t *universe);
universe_t *universe_printstate(universe_t *universe);
int         universe_simulate(universe_t *universe, const args_t *args);
universe_t *universe_init(universe_t *universe, const args_t *args);
void        universe_clean(universe_t *universe);
universe_t *universe_iterate(universe_t *universe, const args_t *args);

#endif
