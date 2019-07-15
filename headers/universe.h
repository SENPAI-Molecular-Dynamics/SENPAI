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

#define UNIVERSE_META_NAME_DEFAULT    "??"
#define UNIVERSE_META_AUTHOR_DEFAULT  "??"
#define UNIVERSE_META_COMMENT_DEFAULT "??"

typedef struct atom_s atom_t;
struct atom_s
{
  uint8_t element;         /* Chemical element */
  double charge;           /* Electrostatic charge (elementary charges) */
  double epsilon;          /* Lennard-Jones parameter (kJ.mol-1) */
  double sigma;            /* Lennard-Jones parameter (angstrom) */

  uint8_t bond_nb;         /* Number of covalent bonds */
  uint64_t *bond;          /* IDs of the bonded atoms */
  double *bond_strength;   /* Strength of each bond (spring constant) */

  vec3d_t pos;             /* Position */
  vec3d_t vel;             /* Velocity */
  vec3d_t acc;             /* Acceleration */
  vec3d_t frc;             /* Force */
};

typedef struct universe_s universe_t;
struct universe_s
{
  /* Metadata from the input file */
  char *meta_name;      /* The name of the system */
  char *meta_author;    /* Who made the file */
  char *meta_comment;   /* Something from the author */

  uint64_t ref_atom_nb; /* The number of atoms in the loaded system */
  uint64_t ref_bond_nb; /* The number of covalent bonds in the loaded system */

  uint64_t copy_nb;     /* Number of copies of the loaded system to simulate */
  uint64_t atom_nb;     /* Total number of atoms in the universe */

  double size;          /* The universe is a cube, that's how long a side is (m) */
  double time;          /* Current time (s) */
  double temperature;   /* Initial thermodynamic temperature (K) */
  double pressure;      /* Initial pressure (Pa) */
  uint64_t iterations;  /* How many iterations have been rendered so far */

  FILE *output_file;    /* The output file (.xyz) */
  FILE *input_file;     /* The input file (.nh4) */

  atom_t *ref_atom;     /* The original system loaded from the file */
  atom_t *atom;         /* The universe to simulate*/
};

void        atom_init(atom_t *atom);
void        atom_clean(atom_t *atom);
int         atom_is_bonded(universe_t *universe, const uint8_t a1, const uint8_t a2);
universe_t *atom_update_frc_numerical(universe_t *universe, const uint64_t part_id);
universe_t *atom_update_frc_analytical(universe_t *universe, const uint64_t part_id);
universe_t *atom_update_acc(universe_t *universe, const uint64_t part_id);
universe_t *atom_update_vel(universe_t *universe, const args_t *args, const uint64_t part_id);
universe_t *atom_update_pos(universe_t *universe, const args_t *args, uint64_t part_id);
universe_t *atom_enforce_pbc(universe_t *universe, const uint64_t part_id);

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
