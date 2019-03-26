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
#include <constants.h>
#include <text.h>

int main(int argc, char **argv)
{
  t_universe universe;
  time_t start_time;
  (void)argc;
  (void)argv;

  universe_init(&universe);

  universe.particle[0].mass = 10E-27;
  universe.particle[0].charge = 2;
  universe.particle[0].pos.x = 10E-12;
  
  universe.particle[1].mass = 6.646481525E-27;
  universe.particle[1].charge = 2;

  fprintf(stdout, TEXT_SIMSTART, C_PART_NB, universe.c_time);
  start_time = time(NULL);
  for (universe.time=0.0; universe.time<C_MAXTIME; universe.time += universe.c_time)
    universe_iterate(&universe);
  fprintf(stdout, TEXT_SIMEND, time(NULL)-start_time);

  return (EXIT_SUCCESS);
}
