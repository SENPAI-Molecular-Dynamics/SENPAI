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
#include <particle.h>
#include <constants.h>
#include <text.h>

int main(int argc, char **argv)
{
  t_universe universe;
  size_t i;
  time_t start_time;
  (void)argc;
  (void)argv;

  universe_init(&universe);

  fprintf(stdout, TEXT_SIMSTART, C_PART_NB, universe.c_time);
  start_time = time(NULL);
  for (universe.time=0.0; universe.time<C_MAXTIME; universe.time += universe.c_time)
    for (i=0; i<C_PART_NB; ++i)
      universe_updateparticle(&universe, i);
  fprintf(stdout, TEXT_SIMEND, time(NULL)-start_time);

  return (EXIT_SUCCESS);
}
