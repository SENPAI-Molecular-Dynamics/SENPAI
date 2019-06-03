/*
 * args.h
 *
 * Licensed under MIT license
 *
 */

#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

#define ARGS_TIMESTEP_DEFAULT    (double)1E-12    /* Use 1 picosecond steps */
#define ARGS_MAX_TIME_DEFAULT    (double)1E-6     /* Simulate 1 microsecond */
#define ARGS_TEMPERATURE_DEFAULT (uint64_t)27315E-2
#define ARGS_MOLECULES_DEFAULT   (uint64_t) 1

#define FLAG_INPUT    "--in"
#define FLAG_OUTPUT   "--out"
#define FLAG_TIME     "--time"
#define FLAG_TEMP     "--temp"
#define FLAG_TIMESTEP "--dt"
#define FLAG_MOL      "--mol"

typedef struct s_args t_args;
struct s_args
{
  char *path;
  char *out_path;
  double timestep;  
  double max_time;
  double temperature;
  uint64_t part_nb;
  uint64_t molecules;
};

t_args *args_init(t_args *args);
t_args *args_parse(t_args *args, int argc, char **argv);

#endif
