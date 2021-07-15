/*
 * args.c
 *
 * Licensed under GPLv3 license
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "args.h"
#include "util.h"
#include "text.h"

args_t *args_init(args_t *args)
{
  if (args == NULL)
  {
    return (retstr(NULL, TEXT_ARGS_INIT_FAILURE, __FILE__, __LINE__));
  }

  args->path_substrate = ARGS_PATH_SUBSTRATE_DEFAULT;
  args->path_out = ARGS_PATH_OUT_DEFAULT;
  args->path_solvent = ARGS_PATH_SOLVENT_DEFAULT;
  args->path_model = ARGS_PATH_MODEL_DEFAULT;
  args->numerical = ARGS_NUMERICAL_DEFAULT;
  args->timestep = ARGS_TIMESTEP_DEFAULT;
  args->max_time = ARGS_MAX_TIME_DEFAULT;
  args->temperature = ARGS_TEMPERATURE_DEFAULT;
  args->copies = ARGS_COPIES_DEFAULT;
  args->pressure = ARGS_PRESSURE_DEFAULT;
  args->density = ARGS_DENSITY_DEFAULT;
  args->frameskip = ARGS_FRAMESKIP_DEFAULT;
  args->reduce_potential = ARGS_REDUCE_POTENTIAL_DEFAULT;
  return (args);
}

args_t *args_check(args_t *args)
{
  /* A model MUST be specified */
  if (args->path_model == ARGS_PATH_MODEL_DEFAULT)
  {
    return (retstr(NULL, TEXT_ARGS_MODEL_FAILURE, __FILE__, __LINE__));
  }

  /* A substrate path MUST be specified */
  if (args->path_substrate == ARGS_PATH_SUBSTRATE_DEFAULT)
  {
    return (retstr(NULL, TEXT_ARGS_SUBSTRATE_FAILURE, __FILE__, __LINE__));
  }
  
  /* A solvent path MUST be specified */
  if (args->path_solvent == ARGS_PATH_SOLVENT_DEFAULT)
  {
    return (retstr(NULL, TEXT_ARGS_SOLVENT_FAILURE, __FILE__, __LINE__));
  }

  /* And so must an output path */
  if (args->path_out == ARGS_PATH_OUT_DEFAULT)
  {
    return (retstr(NULL, TEXT_ARGS_OUT_PATH_FAILURE, __FILE__, __LINE__));
  }

  /* And negative timesteps */
  /* (I'm actually quite tempted to try out negative timesteps...) */
  if (args->timestep <= 0.0)
  {
    return (retstr(NULL, TEXT_ARGS_TIMESTEP_FAILURE, __FILE__, __LINE__));
  }

  /* Simulations shorter than 1 timestep are forbidden */
  if ((args->max_time) <= (args->timestep))
  {
    return (retstr(NULL, TEXT_ARGS_TIME_FAILURE, __FILE__, __LINE__));
  }

  /* There cannot be a null or negative number of particles to simulate */
  if ((args->copies) <= 0)
  {
    return (retstr(NULL, TEXT_ARGS_COPY_FAILURE, __FILE__, __LINE__));
  }

  /* Negative temperatures are invalid */
  if (args->temperature < 0.0)
  {
    return (retstr(NULL, TEXT_ARGS_TEMPERATURE_FAILURE, __FILE__, __LINE__));
  }

  /* The pressure cannot be negative */
  if (args->pressure <= 0.0)
  {
    return (retstr(NULL, TEXT_ARGS_PRESSURE_FAILURE, __FILE__, __LINE__));
  }

  /* Just like the density */
  if (args->density <= 0.0)
  {
    return (retstr(NULL, TEXT_ARGS_DENSITY_FAILURE, __FILE__, __LINE__));
  }

  /* A negative potential has no meaning here */
  if (args->reduce_potential <= 0.0)
  {
    return (retstr(NULL, TEXT_ARGS_REDUCEPOT_FAILURE, __FILE__, __LINE__));
  }

  return (args);
}

args_t *args_parse(args_t *args, int argc, char **argv)
{
  int64_t i;

  for (i=1; i<argc; ++i)
  {
    if (!strcmp(argv[i], FLAG_SUBSTRATE) && (i+1)<argc)
    {
      args->path_substrate = argv[++i];
    }

    else if (!strcmp(argv[i], FLAG_OUTPUT) && (i+1)<argc)
    {
      args->path_out = argv[++i];
    }

    else if (!strcmp(argv[i], FLAG_SOLVENT) && (i+1)<argc)
    {
      args->path_solvent = argv[++i];
    }

    else if (!strcmp(argv[i], FLAG_NUMERICAL))
    {
      args->numerical = MODE_NUMERICAL;
    }

    else if (!strcmp(argv[i], FLAG_TIME) && (i+1)<argc)
    {
      args->max_time = atof(argv[++i]);
    }

    else if (!strcmp(argv[i], FLAG_TIMESTEP) && (i+1)<argc)
    {
      args->timestep = atof(argv[++i]);
    }

    else if (!strcmp(argv[i], FLAG_COPIES) && (i+1)<argc)
    {
      args->copies = strtoul(argv[++i], NULL, 10);
    }

    else if (!strcmp(argv[i], FLAG_TEMP) && (i+1)<argc)
    {
      args->temperature = atof(argv[++i]);
    }

    else if (!strcmp(argv[i], FLAG_PRESSURE) && (i+1)<argc)
    {
      args->pressure = atof(argv[++i]);
    }

    else if (!strcmp(argv[i], FLAG_DENSITY) && (i+1)<argc)
    {
      args->density = atof(argv[++i]);
    }

    else if (!strcmp(argv[i], FLAG_FRAMESKIP) && (i+1)<argc)
    {
      args->frameskip = strtoul(argv[++i], NULL, 10);
    }

    else if (!strcmp(argv[i], FLAG_REDUCEPOT) && (i+1)<argc)
    {
      args->reduce_potential = atof(argv[++i]);
    }

    else if (!strcmp(argv[i], FLAG_MODEL) && (i+1)<argc)
    {
      args->path_model = argv[++i];
    }

    else if (i == (argc-1))
    {
      printf(TEXT_ARG_INVALIDARG, argv[i]);
      return (retstr(NULL, TEXT_ARGS_PARSE_FAILURE, __FILE__, __LINE__));
    }
  }

  /* Convert units */
  args->max_time *= 1E-9;          /* Scale from ns to s */
  args->timestep *= 1E-15;         /* Scale from fs to s */
  args->pressure *= 1E2;           /* Scale from mbar to Pa */
  args->density  *= 1E3;           /* Scale from g.cm-1 to kg.m-1 */
  args->reduce_potential *= 1E-12; /* Scale from pJ to J */

  if (args_check(args) == NULL)
  {
    return (retstr(NULL, TEXT_ARGS_PARSE_FAILURE, __FILE__, __LINE__));
  }

  return (args);
}
