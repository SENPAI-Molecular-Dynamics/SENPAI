/*
 * args.h
 *
 * Licensed under MIT license
 *
 */

#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

#define ARGS_NUMERICAL_DEFAULT   MODE_ANALYTICAL      /* (unitless) Force computation mode */
#define ARGS_TIMESTEP_DEFAULT    (double)1            /* (fs)       Simulation timestep */
#define ARGS_MAX_TIME_DEFAULT    (double)1            /* (ns)       Simulation duration */
#define ARGS_COPIES_DEFAULT      (uint64_t)1          /* (unitless) Substrate copies to be simulated*/
#define ARGS_TEMPERATURE_DEFAULT (uint64_t)273.15     /* (K)        Universe thermodynamic temperature */
#define ARGS_PRESSURE_DEFAULT    (double)1            /* (hPa)      Universe pressure */
#define ARGS_DENSITY_DEFAULT     (double)1            /* (g.cm-3)   Universe density */
#define ARGS_FRAMESKIP_DEFAULT   (uint64_t)0          /* (unitless) Frameskip */
#define ARGS_REDUCEPOT_DEFAULT   (double)10.0         /* (pJ)       Maximum potential energy before simulating */

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
  uint64_t copies;           /* Number of substrates to be loaded */
  double temperature;        /* thermodynamic temperature of the universe */
  double pressure;           /* Universe pressure, used for gas-phase simulations */
  double density;            /* Universe density, used for solid/liquide phase simulations */
};

args_t *args_init(args_t *args);
args_t *args_check(args_t *args);
args_t *args_parse(args_t *args, int argc, char **argv);

#endif
