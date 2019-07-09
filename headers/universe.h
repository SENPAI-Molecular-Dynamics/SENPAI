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
  uint8_t element; /* The element in question */
  uint8_t bond_nb;

  double charge;
  double epsilon;
  double sigma;

  particle_t *bond[7]; /* Points to the bonded particles  */
  int64_t bond_id[7]; /* IDs of the bonded particles */
  double bond_strength[7]; /* Strength of each bond (spring constant) */

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
universe_t *particle_enforce_pbc(universe_t *universe, const uint64_t part_id);
int         particle_is_bonded(const particle_t *p1, const particle_t *p2);

universe_t *universe_init(universe_t *universe, const args_t *args);
void        universe_clean(universe_t *universe);

universe_t *universe_populate(universe_t *universe);
universe_t *universe_setvelocity(universe_t *universe);
universe_t *universe_load(universe_t *universe);
universe_t *universe_printstate(universe_t *universe);
universe_t *universe_simulate(universe_t *universe, const args_t *args);
universe_t *universe_iterate(universe_t *universe, const args_t *args);

universe_t *universe_energy_kinetic(universe_t *universe, double *energy);
universe_t *universe_energy_potential(universe_t *universe, double *energy);
universe_t *universe_energy_total(universe_t *universe, double *energy);

universe_t *universe_montecarlo(universe_t *universe);

#endif
