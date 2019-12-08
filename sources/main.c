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

#include <mpi.h>


#define MODE_NUMERICAL           1


int main(int argc, char **argv)
{
  // Initializare MPI
  MPI_Init(&argc, &argv);

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
  }

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

  /* Let's roll */

  // universe_simulate(&universe, &args);
  uint64_t frame_nb; /* Used for frameskipping */

  /* Tell the user the simulation is starting */
  puts(TEXT_SIMSTART);

  /* While we haven't reached the target time, we iterate the universe */
  frame_nb = 0;


        // Impartim numarul la cate procese avem
        int ratio = universe.atom_nb/world_size;
        int start = world_rank * ratio;
        int end = (world_rank + 1) * ratio;

      // Daca e ultimul proces, atunci merge pana la final
      if (world_rank == world_size - 1)
        end = universe.atom_nb;

  while (universe.time < args.max_time)
  {
    if(world_rank == 0) {
      /* Print the state to the .xyz file, if required */
     if (!frame_nb)
     {
       universe_printstate(&universe);
       frame_nb = (args.frameskip);
     }
     else
       --frame_nb;
   }
    /* Iterate */
    // 
    // universe_iterate(universe, args)
    size_t i; /* Iterator */

    /* We update the position vector first, as part of the Velocity-Verley integration */
    for (i=0; i<(universe.atom_nb); ++i)
      if (atom_update_pos(&universe, &args, i) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

    /* We enforce the periodic boundary conditions */

    for (i=0; i<(universe.atom_nb); ++i)
      if (atom_enforce_pbc(&universe, i) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

    /* Update the force vectors */
    /* By numerically differentiating the potential energy... */
    if (args.numerical == MODE_NUMERICAL)
    {
      for (i=0; i<(universe.atom_nb); ++i)
        if (atom_update_frc_numerical(&universe, i) == NULL)
          return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
    }

    /* Or analytically solving for force */
    else
    {

      for (i=start; i<end; ++i)
        if (atom_update_frc_analytical(&universe, i) == NULL)
          return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
    }

    /* Update the acceleration vectors */
    for (i=0; i<(universe.atom_nb); ++i)
      if (atom_update_acc(&universe, i) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

    /* Update the speed vectors */
    for (i=0; i<(universe.atom_nb); ++i)
      if (atom_update_vel(&universe, &args, i) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

    // return (universe);

    //TODO broadcast la universe.atom->pos - pentru toti atomii modificati de proces

    double buf[3];
    for (i=0; i<(universe.atom_nb); ++i){
        buf[0] = universe.atom[i].pos.x;
        buf[1] = universe.atom[i].pos.y;
        buf[2] = universe.atom[i].pos.z;
        MPI_Bcast(&buf, 3, MPI_DOUBLE, i/ratio, MPI_COMM_WORLD);
        universe.atom[i].pos.x = buf[0];
        universe.atom[i].pos.y = buf[1];
        universe.atom[i].pos.z = buf[2];
    }
    universe.time += args.timestep;
    ++(universe.iterations);
  }

  /* End of simulation */
  puts(TEXT_SIMEND);
  universe_clean(&universe);

  // Finalizare MPI
  MPI_Finalize();

  return 0;

}
