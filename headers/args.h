/*
 * args.h
 *
 * Licensed under MIT license
 *
 */

#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

#define FLAG_NUMERICAL  "--numerical"
#define FLAG_INPUT      "--in"
#define FLAG_OUTPUT     "--out"
#define FLAG_TIMESTEP   "--dt"
#define FLAG_TIME       "--time"
#define FLAG_COPIES     "--copy"
#define FLAG_TEMP       "--temp"
#define FLAG_PRESSURE   "--pressure"
#define FLAG_DENSITY    "--density"
#define FLAG_FRAMESKIP  "--frameskip"
#define FLAG_REDUCEPOT  "--reduce_potential"
#define FLAG_SOLVENT    "--solvent"

typedef struct args_s args_t;
struct args_s
{
  /* Simulation parameters */
  char *path_substrate;             /* Path to input file */
  char *path_out;            /* Path to output file */
  char *path_solvent;        /* Path to solvent file */
  double timestep;           /* (s)        Simulation timestep */
  double max_time;           /* (s)        Simulation duration */
  double reduce_potential;   /* (pJ)       Maximum potential energy before simulating */
  uint64_t frameskip;        /* (unitless) Frameskip */
  uint8_t numerical;         /* (unitless) Force computation mode */

  /* Chemical properties, thermodynamics */
  uint64_t copies;           /* (unitless) Substrate copies to be simulated */
  double temperature;        /* (K)        Universe thermodynamic temperature */
  double pressure;           /* (hPa)      Universe pressure */
  double density;            /* (g.cm-3)   Universe density */
};

args_t *args_init(args_t *args);
args_t *args_check(args_t *args);
args_t *args_parse(args_t *args, int argc, char **argv);

#endif
