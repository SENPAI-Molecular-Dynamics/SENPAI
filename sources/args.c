/*
 * args.c
 *
 * Licensed under MIT license
 *
 */

#include <args.h>
#include <util.h>
#include <text.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

t_args *args_init(t_args *args)
{
  if (args == NULL)
    return (retstr(NULL, TEXT_ARGS_INIT_FAILURE, __FILE__, __LINE__));

  args->path = NULL;
  args->out_path = NULL;
  args->cnst_elec = ARGS_CNST_ELEC_DEFAULT;
  args->cnst_time = ARGS_CNST_TIME_DEFAULT;
  args->max_time = ARGS_MAX_TIME_DEFAULT;
  return (args);
}

t_args *args_parse(t_args *args, int argc, char **argv)
{
  int64_t i;

  for (i=1; i<argc; ++i)
  {
    if ((!strcmp(argv[i], "--in") || !strcmp(argv[i], "-i")) && (i+1)<argc)
      args->path = argv[++i];
    else if ((!strcmp(argv[i], "--out") || !strcmp(argv[i], "-o")) && (i+1)<argc)
      args->out_path = argv[++i];
    else if ((!strcmp(argv[i], "--elec") || !strcmp(argv[i], "-e")) && (i+1)<argc)
      args->cnst_elec = atof(argv[++i]);
    else if ((!strcmp(argv[i], "--time") || !strcmp(argv[i], "-t")) && (i+1)<argc)
      args->max_time = atof(argv[++i])*1E-12; /* The value is again give in ps */
    else if ((!strcmp(argv[i], "--dt") && (i+1)<argc))
      args->cnst_time = atof(argv[++i])*1E-12; /* The value is given in ps */
  }

  if (args->path == NULL || args->out_path == NULL)
    return (retstr(NULL, TEXT_ARGS_PARSE_FAILURE, __FILE__, __LINE__));
  return (args);
}
