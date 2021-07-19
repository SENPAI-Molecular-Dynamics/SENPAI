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

/* Default substrate metadata */
#define UNIVERSE_META_NAME_DEFAULT    "Unknown substrate"
#define UNIVERSE_META_AUTHOR_DEFAULT  "Unknown author"
#define UNIVERSE_META_COMMENT_DEFAULT "No comment"

typedef struct atom_s atom_t;
struct atom_s
{
  /* MISC. INFORMATION */
  uint8_t element;         /* Chemical element (as defined in model.h) */

  /* BONDS */
  uint8_t bond_nb;         /* Number of covalent bonds */
  uint64_t *bond;          /* IDs of the bonded atoms */

  /* INTERACTIONS */
  double *bond_strength;   /* (N.m-1) Bond stength (indexed by bond nb) */
  double charge;           /* (C)    Electric charge */
  double epsilon;          /* (kJ.mol-1) Internuclear potential well depth */
  double sigma;            /* (Å)        Internuclear equilibrium distance */

  /* MECHANICS */
  vec3d_t pos;             /* Position */
  vec3d_t vel;             /* Velocity */
  vec3d_t acc;             /* Acceleration */
  vec3d_t frc;             /* Force */
};

typedef struct universe_s universe_t;
struct universe_s
{
  /* MISC. INFORMATION */
  FILE *file_output;          /* The output file (.xyz) */
  FILE *file_substrate;       /* The substrate file (.mol) */
  FILE *file_solvent;         /* The solvent file (.mol) */
  uint64_t iterations;        /* How many iterations have been rendered so far */

  /* SUBSTRATE METADATA */
  char *meta_substrate_name;            /* The name of the substrate */
  char *meta_substrate_author;          /* Who made the file */
  char *meta_substrate_comment;         /* Some message from the author */

  /* SUBSTRATE */
  uint64_t substrate_atom_nb; /* The number of atoms in the substrate */
  uint64_t substrate_bond_nb; /* The number of covalent bonds in the substrate */
  atom_t *substrate_atom;     /* The substrate atoms as loaded from the file */

  /* SOLVENT METADATA */
  char *meta_solvent_name;    /* The name of the solvent */
  char *meta_solvent_author;  /* Who made the file */
  char *meta_solvent_comment; /* Some message from the author */

  /* SOLVENT */
  uint64_t solvent_atom_nb;   /* The number of atom in the solvent */
  uint64_t solvent_bond_nb;   /* The number of covalent bonds in the solvent */
  atom_t *solvent_atom;       /* The solvent atoms as loaded from the file */
  
  /* UNIVERSE */
  atom_t *atom;               /* The universe (set of all atoms) to simulate */
  uint64_t copy_nb;           /* Number of copies of the substrate to simulate */
  uint64_t atom_nb;           /* Total number of atoms in the universe */

  /* PARAMETERS & THERMODYNAMICS */
  double size;                /* (m) The universe is a cube, that's how long a side is */
  double time;                /* (s) Current time */
  double temperature;         /* (K) Initial thermodynamic temperature */
  double pressure;            /* (Pa) Initial pressure */
};

/* ################## */
/* # ATOM FUNCTIONS # */
/* ################## */
/* The following functions operate on the atom_s structure */
void        atom_init(atom_t *atom);
void        atom_clean(atom_t *atom);
int         atom_is_bonded(universe_t *universe, const uint8_t a1, const uint8_t a2);
universe_t *atom_update_frc_numerical(universe_t *universe, const uint64_t atom_id);
universe_t *atom_update_frc_numerical_tetrahedron(universe_t *universe, const uint64_t atom_id);
universe_t *atom_update_frc_analytical(universe_t *universe, const uint64_t atom_id);
universe_t *atom_update_acc(universe_t *universe, const uint64_t atom_id);
universe_t *atom_update_vel(universe_t *universe, const args_t *args, const uint64_t atom_id);
universe_t *atom_update_pos(universe_t *universe, const args_t *args, uint64_t atom_id);
universe_t *atom_enforce_pbc(universe_t *universe, const uint64_t atom_id);

/* ###################### */
/* # UNIVERSE FUNCTIONS # */
/* ###################### */
/* The following functions operate on the universe_s structure */
universe_t *universe_init(universe_t *universe, const args_t *args);
void        universe_clean(universe_t *universe);
universe_t *universe_populate(universe_t *universe);
universe_t *universe_setvelocity(universe_t *universe);
universe_t *universe_load_substrate(universe_t *universe, char *substrate_file_buffer);
universe_t *universe_load_solvent(universe_t *universe, char *solvent_file_buffer);
universe_t *universe_printstate(universe_t *universe);
int         universe_simulate(universe_t *universe, const args_t *args);
universe_t *universe_iterate(universe_t *universe, const args_t *args);
universe_t *universe_energy_kinetic(universe_t *universe, double *energy);
universe_t *universe_energy_potential(universe_t *universe, double *energy);
universe_t *universe_energy_total(universe_t *universe, double *energy);
universe_t *universe_reducepot(universe_t *universe, args_t *args);
universe_t *universe_reducepot_coarse(universe_t *universe);
universe_t *universe_reducepot_fine(universe_t *universe);
universe_t *universe_parameters_print(universe_t *universe, const args_t *args);

#endif
