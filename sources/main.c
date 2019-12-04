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
  // Initializare MPI
  MPI_Init(argc, argv);

  args_t args;         /* Program arguments (from argv) */
  universe_t universe; /* The universe itself (wow) */

  /* We don't need a perfectly random generator */
  // srand((unsigned int)time(NULL));


  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  /* That's the welcome message */
  if(world_rank == 0) {
    puts(TEXT_START);

    /* Parse the arguments */
    args_init(&args);
    if (args_parse(&args, argc, argv) == NULL)
      return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));

    /* Initialise the universe with the arguments */
    if (universe_init(&universe, &args) == NULL)
      return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));

    /* Reduce the potential energy before simulating */
     if (universe_reducepot(&universe, &args) == NULL)
      return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));

    /* Print the simulation parameters */
    if (universe_parameters_print(&universe, &args) == NULL)
      return (retstri(EXIT_FAILURE, TEXT_MAIN_FAILURE, __FILE__, __LINE__));
  }

  /* Let's roll */

  // universe_simulate(&universe, &args);
  uint64_t frame_nb; /* Used for frameskipping */

  /* Tell the user the simulation is starting */
  puts(TEXT_SIMSTART);

  /* While we haven't reached the target time, we iterate the universe */
  frame_nb = 0;
  while (universe->time < args->max_time)
  {
    if(world_rank == 0) {
      /* Print the state to the .xyz file, if required */
     if (!frame_nb)
     {
       universe_printstate(universe)
       frame_nb = (args->frameskip);
     }
     else
       --frame_nb;
   }
    /* Iterate */
    // 
    universe_iterate(universe, args)

    universe->time += args->timestep;
    ++(universe->iterations);
  }

  /* End of simulation */
  puts(TEXT_SIMEND);
  universe_clean(universe);

  // Finalizare MPI - maybe trebuie pus inainte de fiecare return
  MPI_Finalize();

  return 0;

}
