/*
 * args.h
 *
 * Licensed under MIT license
 *
 */

#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

#define ARGS_NUMERICAL_DEFAULT   MODE_ANALYTICAL      /* Disable numerical differentiation solving by default */
#define ARGS_TIMESTEP_DEFAULT    (double)1            /* Use 1 femtosecond steps */
#define ARGS_MAX_TIME_DEFAULT    (double)1            /* Simulate 1 nanosecond */
#define ARGS_COPIES_DEFAULT      (uint64_t)1          /* Number of copies of the loaded system */
#define ARGS_TEMPERATURE_DEFAULT (uint64_t)273.15     /* Temperature of the universe */
#define ARGS_PRESSURE_DEFAULT    (double)1            /* Pressure of the universe */
#define ARGS_DENSITY_DEFAULT     (double)1            /* One gram per cubic centimetre */
#define ARGS_FRAMESKIP_DEFAULT   (uint64_t)0          /* Frameskip */
#define ARGS_REDUCEPOT_DEFAULT   (double)10.0         /* Maximum potential energy before simulating (pJ) */

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

typedef struct args_s args_t;
struct args_s
{
  /* Simulation parameters */
  char *path;                /* Path to input file */
  char *out_path;            /* Path to output file */
  double timestep;           /* Timestep used during integration */
  double max_time;           /* Time at which the simulation stops */
  double reduce_potential;   /* Number of potential reduction cycles to run */
  uint64_t frameskip;        /* How many frames to skip */
  uint8_t numerical;         /* Computation mode */

  /* Chemical properties, thermodynamics */
  uint64_t copies;
  double temperature;
  double pressure;
  double density;
};

args_t *args_init(args_t *args);
args_t *args_check(args_t *args);
args_t *args_parse(args_t *args, int argc, char **argv);

#endif
