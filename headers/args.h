/*
 * args.h
 *
 * Licensed under MIT license
 *
 */

#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

#define ARGS_CNST_ELEC_DEFAULT (double)(8.98755*10E9)
#define ARGS_CNST_TIME_DEFAULT (double)1E-12 /* Use 1 picosecond steps */
#define ARGS_MAX_TIME_DEFAULT  (double)1E-6 /* Simulate 1 microsecond */

typedef struct s_args t_args;
struct s_args
{
  char *path;
  char *out_path;

  double cnst_elec;
  double cnst_time;
  
  double max_time;
  uint64_t part_nb;
};

t_args *args_init(t_args *args);
t_args *args_parse(t_args *args, int argc, char **argv);

#endif
