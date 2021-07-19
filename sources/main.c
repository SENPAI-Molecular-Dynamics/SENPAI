/*
 * main.c
 *
 * Licensed under GPLv3 license
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
  args_t args;         /* Program arguments (from argv) */
  universe_t universe; /* The universe itself (wow) */

  /* We don't need a perfectly random generator */
  srand((unsigned int)time(NULL));

  /* That's the welcome message */
  puts(TEXT_START);

  /* Parse the arguments */
  args_init(&args);
  if (args_parse(&args, argc, argv) == NULL)
  {
    return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));
  }

  /* Initialise the universe with the arguments */
  if (universe_init(&universe, &args) == NULL)
  {
    return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));
  }

  /* Reduce the potential energy before simulating */
   if (universe_reducepot(&universe, &args) == NULL)
  {
    return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));
  }

  /* Print the simulation parameters */
  if (universe_parameters_print(&universe, &args) == NULL)
  {
    return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));
  }

  /* Let's roll */
  return (universe_simulate(&universe, &args));
}
