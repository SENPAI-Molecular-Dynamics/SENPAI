/*
 * args.c
 *
 * Licensed under MIT license
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "args.h"
#include "util.h"
#include "text.h"

args_t *args_init(args_t *args)
{
  if (args == NULL)
    return (retstr(NULL, TEXT_ARGS_INIT_FAILURE, __FILE__, __LINE__));

  args->path = NULL;
  args->out_path = NULL;
  args->numerical = ARGS_NUMERICAL_DEFAULT;
  args->timestep = ARGS_TIMESTEP_DEFAULT;
  args->max_time = ARGS_MAX_TIME_DEFAULT;
  args->temperature = ARGS_TEMPERATURE_DEFAULT;
  args->copies = ARGS_COPIES_DEFAULT;
  args->pressure = ARGS_PRESSURE_DEFAULT;
  args->frameskip = ARGS_FRAMESKIP_DEFAULT;
  args->montecarlo = ARGS_MONTECARLO_DEFAULT;
  return (args);
}

args_t *args_parse(args_t *args, int argc, char **argv)
{
  int64_t i;

  for (i=1; i<argc; ++i)
  {
    if (!strcmp(argv[i], FLAG_INPUT) && (i+1)<argc)
      args->path = argv[++i];
    else if (!strcmp(argv[i], FLAG_OUTPUT) && (i+1)<argc)
      args->out_path = argv[++i];
    else if (!strcmp(argv[i], FLAG_NUMERICAL))
      args->numerical = MODE_NUMERICAL;
    else if (!strcmp(argv[i], FLAG_TIME) && (i+1)<argc)
      args->max_time = atof(argv[++i])*1E-9; /* Scale from nanoseconds */
    else if (!strcmp(argv[i], FLAG_TIMESTEP) && (i+1)<argc)
      args->timestep = atof(argv[++i])*1E-15; /* Scale from femtoseconds */
    else if (!strcmp(argv[i], FLAG_TEMP) && (i+1)<argc)
      args->temperature = atof(argv[++i]);
    else if (!strcmp(argv[i], FLAG_COPIES) && (i+1)<argc)
      args->copies = strtoul(argv[++i], NULL, 10);
    else if (!strcmp(argv[i], FLAG_PRESSURE) && (i+1)<argc)
      args->pressure = atof(argv[++i])*1E2; /* Scale from mbar */
    else if (!strcmp(argv[i], FLAG_FRAMESKIP) && (i+1)<argc)
      args->frameskip = strtoul(argv[++i], NULL, 10);
    else if (!strcmp(argv[i], FLAG_MONTECARLO) && (i+1)<argc)
      args->montecarlo = strtoul(argv[++i], NULL, 10);
  }

  if (args->path == NULL ||
      args->out_path == NULL ||
      args->copies < 1 ||
      args->temperature <= 0.0 ||
      args->pressure < 0.0 ||
      args->timestep < 0.0)
    return (retstr(NULL, TEXT_ARGS_PARSE_FAILURE, __FILE__, __LINE__));
  return (args);
}
