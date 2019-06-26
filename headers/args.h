/*
 * args.h
 *
 * Licensed under MIT license
 *
 */

#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

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
  uint64_t molecules;
  uint64_t frameskip;
};

args_t *args_init(args_t *args);
args_t *args_parse(args_t *args, int argc, char **argv);

#endif
