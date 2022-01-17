/*
 * universe.h
 *
 * Licensed under GPLv3 license
 *
 */

#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <stdint.h>
#include <stdio.h>

#include "model.h"
#include "vec3.h"
#include "text.h"
#include "args.h"

/* t_atom */
#define ATOM_ELEMENT_DEFAULT       ((uint64_t)   0)
#define ATOM_CHARGE_DEFAULT        ((double)     0.0)
#define ATOM_EPSILON_DEFAULT       ((double)     0.0)
#define ATOM_SIGMA_DEFAULT         ((double)     0.0)
#define ATOM_BOND_NB_DEFAULT       ((uint8_t)    0)
#define ATOM_BOND_DEFAULT          ((uint64_t *) NULL)
#define ATOM_BOND_STRENGTH_DEFAULT ((double *)   NULL)
#define ATOM_POS_X_DEFAULT         ((double)     0.0)
#define ATOM_POS_Y_DEFAULT         ((double)     0.0)
#define ATOM_POS_Z_DEFAULT         ((double)     0.0)
#define ATOM_VEL_X_DEFAULT         ((double)     0.0)
#define ATOM_VEL_Y_DEFAULT         ((double)     0.0)
#define ATOM_VEL_Z_DEFAULT         ((double)     0.0)
#define ATOM_ACC_X_DEFAULT         ((double)     0.0)
#define ATOM_ACC_Y_DEFAULT         ((double)     0.0)
#define ATOM_ACC_Z_DEFAULT         ((double)     0.0)
#define ATOM_FRC_X_DEFAULT         ((double)     0.0)
#define ATOM_FRC_Y_DEFAULT         ((double)     0.0)
#define ATOM_FRC_Z_DEFAULT         ((double)     0.0)

/* t_universe */
#define UNIVERSE_FILE_MODEL_DEFAULT             ((FILE*)    NULL)
#define UNIVERSE_FILE_OUTPUT_DEFAULT            ((FILE*)    NULL)
#define UNIVERSE_FILE_SUBSTRATE_DEFAULT         ((FILE*)    NULL)
#define UNIVERSE_FILE_SOLVENT_DEFAULT           ((FILE*)    NULL)
#define UNIVERSE_META_MODEL_NAME_DEFAULT        ((char*)    NULL)
#define UNIVERSE_META_MODEL_AUTHOR_DEFAULT      ((char*)    NULL)
#define UNIVERSE_META_MODEL_COMMENT_DEFAULT     ((char*)    NULL)
#define UNIVERSE_META_SUBSTRATE_NAME_DEFAULT    ((char*)    NULL)
#define UNIVERSE_META_SUBSTRATE_AUTHOR_DEFAULT  ((char*)    NULL)
#define UNIVERSE_META_SUBSTRATE_COMMENT_DEFAULT ((char*)    NULL)
#define UNIVERSE_META_SOLVENT_NAME_DEFAULT      ((char*)    NULL)
#define UNIVERSE_META_SOLVENT_AUTHOR_DEFAULT    ((char*)    NULL)
#define UNIVERSE_META_SOLVENT_COMMENT_DEFAULT   ((char*)    NULL)
#define UNIVERSE_MODEL_ENTRY_NB_DEFAULT         ((uint64_t) 0   )
#define UNIVERSE_SUBSTRATE_ATOM_NB_DEFAULT      ((uint64_t) 0   )
#define UNIVERSE_SUBSTRATE_BOND_NB_DEFAULT      ((uint64_t) 0   )
#define UNIVERSE_SUBSTRATE_ATOM_DEFAULT         ((atom_t*)  NULL)
#define UNIVERSE_SOLVENT_ATOM_NB_DEFAULT        ((uint64_t) 0   )
#define UNIVERSE_SOLVENT_BOND_NB_DEFAULT        ((uint64_t) 0   )
#define UNIVERSE_SOLVENT_ATOM_DEFAULT           ((atom_t*)  NULL)
#define UNIVERSE_ATOM_DEFAULT                   ((atom_t*)  NULL)
#define UNIVERSE_COPY_NB_DEFAULT                ((uint64_t) 0   )
#define UNIVERSE_ATOM_NB_DEFAULT                ((uint64_t) 0   )
#define UNIVERSE_ITERATIONS_DEFAULT             ((uint64_t) 0   )
#define UNIVERSE_SIZE_DEFAULT                   ((double)   0.0 )
#define UNIVERSE_TIME_DEFAULT                   ((double)   0.0 )
#define UNIVERSE_TEMPERATURE_DEFAULT            ((double)   0.0 )
#define UNIVERSE_PRESSURE_DEFAULT               ((double)   0.0 )

typedef struct atom_s atom_t;
struct atom_s
{
  /* MISC. INFORMATION */
  uint64_t element;      /* Chemical element (as defined in model.h) */

  /* BONDS */
  uint8_t bond_nb;       /* Number of covalent bonds */
  uint64_t *bond;        /* IDs of the bonded atoms */

  /* INTERACTIONS */
  double *bond_strength; /* (N.m-1) Bond stength (indexed by bond nb) */
  double charge;         /* (C)    Electric charge */
  double epsilon;        /* (kJ.mol-1) Internuclear potential well depth */
  double sigma;          /* (Å)        Internuclear equilibrium distance */

  /* MECHANICS */
  vec3_t pos;            /* Position */
  vec3_t vel;            /* Velocity */
  vec3_t acc;            /* Acceleration */
  vec3_t frc;            /* Force */
};

typedef struct universe_s universe_t;
struct universe_s
{
  /* MISC. INFORMATION */
  FILE *file_model;             /* The model file (.mdm) */
  FILE *file_output;            /* The output file (.xyz) */
  FILE *file_substrate;         /* The substrate file (.mds) */
  FILE *file_solvent;           /* The solvent file (.mds) */

  /* MODEL METADATA */
  char *meta_model_name;        /* The name of the model */
  char *meta_model_author;      /* Who made the file */
  char *meta_model_comment;     /* Some message from the author */

  /* SUBSTRATE METADATA */
  char *meta_substrate_name;    /* The name of the substrate */
  char *meta_substrate_author;  /* Who made the file */
  char *meta_substrate_comment; /* Some message from the author */

  /* SOLVENT METADATA */
  char *meta_solvent_name;      /* The name of the solvent */
  char *meta_solvent_author;    /* Who made the file */
  char *meta_solvent_comment;   /* Some message from the author */

  /* MODEL */
  model_t model;                /* The model used by the universe */
  uint64_t model_entry_nb;      /* How many entries are in the model */

  /* SUBSTRATE */
  uint64_t substrate_atom_nb;   /* The number of atoms in the substrate */
  uint64_t substrate_bond_nb;   /* The number of covalent bonds in the substrate */
  atom_t *substrate_atom;       /* The substrate atoms as loaded from the file */

  /* SOLVENT */
  uint64_t solvent_atom_nb;     /* The number of atom in the solvent */
  uint64_t solvent_bond_nb;     /* The number of covalent bonds in the solvent */
  atom_t *solvent_atom;         /* The solvent atoms as loaded from the file */
  
  /* UNIVERSE */
  atom_t *atom;                 /* The universe (set of all atoms) to simulate */
  uint64_t copy_nb;             /* Number of copies of the substrate to simulate */
  uint64_t atom_nb;             /* Total number of atoms in the universe */
  uint64_t iterations;          /* How many iterations have been rendered so far */

  /* PARAMETERS & THERMODYNAMICS */
  double size;                  /* (m) The universe is a cube, that's how long a side is */
  double time;                  /* (s) Current time */
  double temperature;           /* (K) Initial thermodynamic temperature */
  double pressure;              /* (Pa) Initial pressure */
};

/* ################## */
/* # ATOM FUNCTIONS # */
/* ################## */
/* The following functions operate on the atom_s structure */
void        atom_init(atom_t *atom);
void        atom_clean(atom_t *atom);
int         atom_is_bonded(universe_t *universe, const uint64_t a1, const uint64_t a2);
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
universe_t *universe_load_model(universe_t *universe, char *model_file_buffer);
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
