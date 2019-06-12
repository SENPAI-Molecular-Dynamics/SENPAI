/*
 * main.c
 *
 * Licensed under MIT license
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "universe.h"
#include "args.h"
#include "text.h"
#include "util.h"

int main(int argc, char **argv)
{
  universe_t universe;
  args_t args;
  int exit_state;

  srand((unsigned int)time(NULL));
  
  /* That's the welcome message */
  puts(TEXT_START);

  /* Parse the arguments */
  args_init(&args);
  if (args_parse(&args, argc, argv) == NULL)
    return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));
  
  /* Initialise the universe with the arguments */
  if (universe_init(&universe, &args) == NULL)
    return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));

  /* Let's roll */
  printf(TEXT_SIMSTART, universe.mol_nb, universe.part_nb, universe.temperature, args.pressure, args.max_time*1E12, args.timestep*1E12);
  exit_state = universe_simulate(&universe, &args);
  printf(TEXT_SIMEND, universe.iterations);

  universe_clean(&universe);
  return (exit_state);
}
