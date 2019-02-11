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

  srand(time(NULL));
  universe_init(&universe);

  for (i=0; i<C_PART_NB; ++i)
  {
    universe.particle[i].mass = 1E24;
    universe.particle[i].pos.x = rand() % 10;
    universe.particle[i].pos.y = rand() % 10;
    universe.particle[i].pos.z = rand() % 10;
  }

  fprintf(stdout, TEXT_SIMSTART, C_PART_NB, universe.c_time);
  start_time = time(NULL);
  for (universe.time=0.0; universe.time<C_MAXTIME; universe.time += universe.c_time)
    for (i=0; i<C_PART_NB; ++i)
      if (universe_updateparticle(&universe, i) == NULL)
      {
        /* print TEXT_SIMFAIL to stderr */
        return (EXIT_FAILURE);
      }
  fprintf(stdout, TEXT_SIMEND, time(NULL)-start_time);
  universe_clean(&universe);

  return (EXIT_SUCCESS);
}
