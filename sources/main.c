/*
 * main.c
 *
 * Licensed under MIT license
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "universe.h"
#include "args.h"
#include "text.h"
#include "util.h"

int main(int argc, char **argv)
{
  size_t i;
  void *exit_state;
  args_t args;
  universe_t universe;

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

  /* Run some Monte Carlo to reduce the system's potential */
  printf(TEXT_MONTECARLO, args.montecarlo);
  for (i=0; i<(args.montecarlo); ++i)
    if (universe_montecarlo(&universe) == NULL)
    return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));

  /* Let's roll */
  exit_state = universe_simulate(&universe, &args);

  universe_clean(&universe);
  if (exit_state == NULL)
    return (EXIT_FAILURE);
  return (EXIT_SUCCESS);
}
