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
  time_t end_time;
  int exit_state;

  args_init(&args);
  if (args_parse(&args, argc, argv) == NULL)
    return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));
  if (universe_init(&universe, &args) == NULL)
    return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));

  fprintf(stdout, TEXT_SIMSTART, universe.part_nb, args.max_time*1E12, universe.c_time*1E12); /* We multiply by 10e12 to give the value in ps */
  start_time = time(NULL);
  exit_state = universe_simulate(&universe, &args);
  end_time = time(NULL)+1;
  fprintf(stdout, TEXT_SIMEND, universe.iterations, end_time-start_time, (double)(universe.iterations/(end_time-start_time)));

  universe_clean(&universe);
  return (exit_state);
}
