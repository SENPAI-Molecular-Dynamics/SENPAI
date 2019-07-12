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
  uint8_t element;         /* Chemical element */
  double charge;           /* Electrostatic charge (elementary charges) */
  double epsilon;          /* Lennard-Jones parameter (kJ.mol-1) */
  double sigma;            /* Lennard-Jones parameter (angstrom) */

  uint8_t bond_nb;         /* Number of covalent bonds */
  particle_t *bond[7];     /* Points to the bonded particles  */
  int64_t bond_id[7];      /* IDs of the bonded particles */
  double bond_strength[7]; /* Strength of each bond (spring constant) */

  vec3d_t pos;             /* Position */
  vec3d_t spd;             /* Velocity */
  vec3d_t acc;             /* Acceleration */
  vec3d_t frc;             /* Force */
};

typedef struct universe_s universe_t;
struct universe_s
{
  uint64_t sys_size;    /* How many atoms in the loaded system */
  uint64_t copy_nb;     /* Number of copies of the loaded system to simulate */
  uint64_t part_nb;     /* Total number of atoms in the universe (=sys_size*copy_nb) */

  double size;          /* The universe is a cube, that's how long a side is (m) */
  double time;          /* Current time (s) */
  double temperature;   /* Initial thermodynamic temperature (K) */
  double pressure;      /* Initial pressure (Pa) */
  uint64_t iterations;  /* How many iterations have been rendered so far */

  FILE *input_file;     /* The input file (.nh4) */
  FILE *output_file;    /* The output file (.xyz) */

  particle_t *particle; /* The particles in the universe */
};

particle_t *particle_init(particle_t *particle);
universe_t *particle_update_frc_numerical(universe_t *universe, const uint64_t part_id);
universe_t *particle_update_frc_analytical(universe_t *universe, const uint64_t part_id);
universe_t *particle_update_acc(universe_t *universe, const uint64_t part_id);
universe_t *particle_update_spd(universe_t *universe, const args_t *args, const uint64_t part_id);
universe_t *particle_update_pos(universe_t *universe, const args_t *args, uint64_t part_id);
universe_t *particle_enforce_pbc(universe_t *universe, const uint64_t part_id);
int         particle_is_bonded(const particle_t *p1, const particle_t *p2);
universe_t *universe_init(universe_t *universe, const args_t *args);
void        universe_clean(universe_t *universe);
universe_t *universe_populate(universe_t *universe);
universe_t *universe_setvelocity(universe_t *universe);
universe_t *universe_load(universe_t *universe, char *input_file_buffer);
universe_t *universe_printstate(universe_t *universe);
universe_t *universe_simulate(universe_t *universe, const args_t *args);
universe_t *universe_iterate(universe_t *universe, const args_t *args);
universe_t *universe_energy_kinetic(universe_t *universe, double *energy);
universe_t *universe_energy_potential(universe_t *universe, double *energy);
universe_t *universe_energy_total(universe_t *universe, double *energy);
universe_t *universe_montecarlo(universe_t *universe);

#endif
