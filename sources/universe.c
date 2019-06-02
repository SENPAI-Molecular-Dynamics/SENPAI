/*
 * universe.c
 *
 * Licensed under MIT license
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "args.h"
#include "text.h"
#include "vec3d.h"
#include "util.h"
#include "universe.h"
#include "bond.h"

t_universe *universe_load(t_universe *universe)
{
  size_t i;
  size_t ii;
  int bond_id[7];
  char *tok;
  t_particle *temp;
  
  /* Load the initial state from the input file */
  tok = strtok(universe->input_file_buffer, "\n");
  for (i=0; i<(universe->part_nb); ++i)
  {
    temp = &(universe->particle[i]);
    if (sscanf(tok,
               "%2s,%lf,%lf,%d,%d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
               temp->element,
               &(temp->mass),
               &(temp->charge),
               &(bond_id[0]),
               &(bond_id[1]),
               &(bond_id[2]),
               &(bond_id[3]),
               &(bond_id[4]),
               &(bond_id[5]),
               &(bond_id[6]),
               &(temp->bond_strength[0]),
               &(temp->bond_strength[1]),
               &(temp->bond_strength[2]),
               &(temp->bond_strength[3]),
               &(temp->bond_strength[4]),
               &(temp->bond_strength[5]),
               &(temp->bond_strength[6]),
               &(temp->bond_length[0]),
               &(temp->bond_length[1]),
               &(temp->bond_length[2]),
               &(temp->bond_length[3]),
               &(temp->bond_length[4]),
               &(temp->bond_length[5]),
               &(temp->bond_length[6]),
               &(temp->pos.x),
               &(temp->pos.y),
               &(temp->pos.z)) < 0)
      return (retstr(NULL, TEXT_INPUTFILE_FAILURE, __FILE__, __LINE__));
    temp->mass *= 1.66053904020E-27; /* We convert the values from atomic mass units to kg */
    temp->charge *= 1.602176634E-19; /* Same with charge, from e to C */
    if (vec3d_mul(&(temp->pos), &(temp->pos), 1E-12) == NULL) /* We convert the position vector from pm to m */
      return (retstr(NULL, TEXT_CANTMATH, __FILE__, __LINE__));

    /* Set up the bonds */
    for (ii=0; ii<7; ++ii)
    {
      temp->bond_length[ii] *= 1E-12;
      if (bond_id[ii] < 0)
        temp->bond[ii] = NULL;
      else
        temp->bond[ii] = &(universe->particle[bond_id[ii]]);
    }
    
    tok = strtok(NULL, "\n");
  }
  return (universe);
}

t_universe *universe_init(t_universe *universe, const t_args *args)
{
  char c;
  size_t i;
  size_t file_len;
  FILE *input_file;

  /* Initialize the variables */
  universe->part_nb = 0;
  universe->time = 0.0;
  universe->iterations = 0;

  /* Open the input file */
  if ((input_file = fopen(args->path, "r")) == NULL)
    return (retstr(NULL, TEXT_INPUTFILE_FAILURE, __FILE__, __LINE__));

  /* Count the lines in the input file to get the nb of particles */
  while ((c = getc(input_file)) != EOF)
     if (c == '\n')
       ++(universe->part_nb);

  /* Get the file's size */
  fseek(input_file, 0, SEEK_END);
  file_len = ftell(input_file);
  rewind(input_file);
  if ((universe->input_file_buffer = (char*)malloc(file_len+1)) == NULL)
    return (retstr(NULL, TEXT_MALLOC_FAILURE, __FILE__, __LINE__));

  /* Load it in memory, terminate the string */
  if (fread(universe->input_file_buffer, sizeof(char), file_len, input_file) != file_len)
    return (retstr(NULL, TEXT_INPUTFILE_FAILURE, __FILE__, __LINE__));
  universe->input_file_buffer[file_len] = '\0';

  /* Close it, we're done */
  fclose(input_file);

  /* Allocate memory for the particles */
  sscanf(universe->input_file_buffer, "ATOMS=%ld\n", &(universe->part_nb));
  if ((universe->particle = malloc((universe->part_nb)*sizeof(t_particle))) == NULL)
    return (retstr(NULL, TEXT_MALLOC_FAILURE, __FILE__, __LINE__));

  /* Initialize the particle memory */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_init(&(universe->particle[i])) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Load the initial state from the input file */
  if (universe_load(universe) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Apply initial velocities */
  if (universe_setvelocity(universe) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Initialize the .xyz file pointer */
  if ((universe->output_file_xyz = fopen(args->out_path, "w")) == NULL)
    return (retstr(NULL, TEXT_OUTPUTFILE_FAILURE, __FILE__, __LINE__));

  printf("Loaded %ld particles\n", universe->part_nb);
  return (universe);
}

t_universe *universe_setvelocity(t_universe *universe)
{
  double kinetic_avg;
  double velocity;
  t_vec3d spd_vec;
  size_t i;
  
  /* Get the average kinetic energy from temperature */
  kinetic_avg = (3/2)*(C_IDEALGAS/C_AVOGADRO)*universe->temp;

  for (i=0; i<(universe->part_nb); ++i)
  {
    /* Generate a random unit vector */
    spd_vec.x = rand();
    spd_vec.y = rand();
    spd_vec.z = rand();
    if (vec3d_div(&spd_vec, &spd_vec, vec3d_mag(&spd_vec)) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_SETVELOCITY_FAILURE, __FILE__, __LINE__));
    
    /* Apply an average speed from the average kinetic energy */
    velocity = sqrt(2*kinetic_avg/(universe->particle[i].mass));
    if (vec3d_mul(&(universe->particle[i].spd), &spd_vec, velocity) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_SETVELOCITY_FAILURE, __FILE__, __LINE__));
  }
  
  return (universe);
}

void universe_clean(t_universe *universe)
{
  /* Close the file pointers */
  fclose(universe->output_file_xyz);

  /* Free allocated memory */
  free(universe->input_file_buffer);
  free(universe->particle);}

t_universe *universe_iterate(t_universe *universe, const t_args *args)
{
  uint64_t i;

  /* We update the position vector first, as part of the Velocity-Verley integration */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_pos(universe, args, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* Update the force vectors */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_frc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* Update the acceleration vectors */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_acc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* Update the speed vectors */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_spd(universe, args, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  universe->iterations++;
  return (universe);
}

int universe_simulate(t_universe *universe, const t_args *args)
{
  /* While we haven't reached the target time, we iterate the universe */
  while (universe->time < args->max_time)
  {
    if (universe_printstate(universe) == NULL)
      return (retstri(EXIT_FAILURE, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));
    if (universe_iterate(universe, args) == NULL)
      return (retstri(EXIT_FAILURE, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));
    universe->time += args->timestep;
    ++(universe->iterations);
  }
  return (EXIT_SUCCESS);
}

t_universe *universe_printstate(t_universe *universe)
{
  size_t i;
  t_particle *p;

  /* Print in the .xyz */
  fprintf(universe->output_file_xyz, "%ld\n%ld\n", universe->part_nb, universe->iterations);
  for (i=0; i<(universe->part_nb); ++i)
  {
    p = &(universe->particle[i]);
    fprintf(universe->output_file_xyz,
            "%s\t%.15lf\t%.15lf\t%.15lf\n",
            p->element,
            p->pos.x*1E10,
            p->pos.y*1E10,
            p->pos.z*1E10);
  }
  return (universe);
}
