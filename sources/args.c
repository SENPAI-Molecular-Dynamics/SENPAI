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
    return (retstr(NULL, TEXT_ARGS_INIT_NULLARG, __FILE__, __LINE__));

  args->path = NULL;
  return (args);
}

t_args *args_parse(t_args *args, int argc, char **argv)
{
  int64_t i;

  i = 1;
  while (i<argc)
  {
    if ((!strcmp(argv[i], "--verbose") || !strcmp(argv[i], "-v")) && (i+1)<argc)
    {
      args->verbose = 1;
      ++i;
    }
    else if ((!strcmp(argv[i], "--path") || !strcmp(argv[i], "-p")) && (i+1)<argc)
      args->path = argv[++i];
    else
      return (retstr(NULL, TEXT_ARGS_PARSE_BADFORMAT, __FILE__, __LINE__));
  }

  return (args);
}
