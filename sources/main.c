/*
 * main.c
 *
 * Licensed under MIT license
 *
 */

#include <stdlib.h>
#include <universe.h>
#include <particle.h>
#include <constants.h>

int main(int argc, char **argv)
{
  t_universe universe;
  size_t i;
  (void)argc;
  (void)argv;

  universe_init(&universe);
  
  universe.particle[0].mass = 5.972E24;
  universe.particle[0].pos = e_0;

  universe.particle[1].mass = 7.342E22;
  universe.particle[1].pos = e_0;
  universe.particle[1].pos.x = 3.844E8;

  for (universe.time=0.0; universe.time<C_MAXTIME; universe.time += universe.c_time)
    for (i=0; i<C_PART_NB; ++i)
      universe_updateparticle(&universe, i);

  return (EXIT_SUCCESS);
}
