/*
 * main.c
 *
 * Licensed under MIT license
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <universe.h>
#include <args.h>
#include <text.h>
#include <util.h>

int main(int argc, char **argv)
{
  t_universe universe;
  t_args args;
  time_t start_time;

  args_init(&args);
  if (args_parse(&args, argc, argv) == NULL)
    return (retstri(EXIT_FAILURE, TEXT_ARGS_PARSE_FAILURE, __FILE__, __LINE__));
  universe_init(&universe, &args);

  fprintf(stdout, TEXT_SIMSTART, universe.part_nb, universe.c_time);
  start_time = time(NULL);
  for (universe.time=0.0; (universe.time)<(args.max_time); universe.time += universe.c_time)
    if (universe_iterate(&universe) == NULL)
      break;
  fprintf(stdout, TEXT_SIMEND, time(NULL)-start_time);

  universe_clean(&universe);
  if (universe.time < args.max_time)
    return (retstri(EXIT_FAILURE, TEXT_SIMFAILURE, __FILE__, __LINE__));
  return (EXIT_SUCCESS);
}
