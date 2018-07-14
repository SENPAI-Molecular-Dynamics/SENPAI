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
  while (universe.c_time < C_MAXTIME)
  {
    for (i=0; i<C_PART_NB; ++i)
    {
      universe_updateparticle(&universe, i);
    }
  }
  return (EXIT_SUCCESS);
}
