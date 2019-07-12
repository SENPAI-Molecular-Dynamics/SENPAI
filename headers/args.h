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
#define ARGS_TIMESTEP_DEFAULT    (double)1E-15        /* Use 1 femtosecond steps */
#define ARGS_MAX_TIME_DEFAULT    (double)1E-9         /* Simulate 1 nanosecond */
#define ARGS_COPIES_DEFAULT      (uint64_t)1          /* Number of copies of the loaded system */
#define ARGS_TEMPERATURE_DEFAULT (uint64_t)273.15     /* Temperature of the universe */
#define ARGS_PRESSURE_DEFAULT    (double)1E5          /* Pressure of the universe */
#define ARGS_FRAMESKIP_DEFAULT   (uint64_t)0          /* Frameskip */
#define ARGS_MONTECARLO_DEFAULT  (uint64_t)0          /* Monte Carlo potential reduction cycles to run before simulating */

#define FLAG_NUMERICAL  "--numerical"
#define FLAG_INPUT      "--in"
#define FLAG_OUTPUT     "--out"
#define FLAG_TIMESTEP   "--dt"
#define FLAG_TIME       "--time"
#define FLAG_COPIES     "--copy"
#define FLAG_TEMP       "--temp"
#define FLAG_PRESSURE   "--pressure"
#define FLAG_FRAMESKIP  "--frameskip"
#define FLAG_MONTECARLO "--montecarlo"

typedef struct args_s args_t;
struct args_s
{
  char *path;
  char *out_path;
  double timestep;  
  double max_time;
  double temperature;
  double pressure;
  uint8_t numerical;
  uint64_t part_nb;
  uint64_t copies;
  uint64_t frameskip;
  uint64_t montecarlo;
};

args_t *args_init(args_t *args);
args_t *args_parse(args_t *args, int argc, char **argv);

#endif
