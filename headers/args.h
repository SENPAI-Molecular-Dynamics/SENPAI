/*
 * args.h
 *
 * Licensed under MIT license
 *
 */

#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

#define ARGS_CNST_GRAV_DEFAULT (double)0.00000000000667408
#define ARGS_CNST_ELEC_DEFAULT (double)8987551787.3681764
#define ARGS_CNST_TIME_DEFAULT (double)1E-12
#define ARGS_MAX_TIME_DEFAULT (double)1E-9
#define ARGS_PART_NB_DEFAULT (long int)2

typedef struct s_args t_args;
struct s_args
{
  char *path;
  char *csv_path;

  double cnst_grav;
  double cnst_elec;
  double cnst_time;
  
  double max_time;
  uint64_t part_nb;
};

t_args *args_init(t_args *args);
t_args *args_parse(t_args *args, int argc, char **argv);

#endif
