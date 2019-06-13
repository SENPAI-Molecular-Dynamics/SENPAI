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
#include "potential.h"

universe_t *universe_init(universe_t *universe, const args_t *args)
{
  int32_t c;
  size_t file_len;
  size_t i;
  FILE *input_file;

  /* Initialize the variables */
  universe->part_nb = 0;
  universe->mol_size = 0;
  universe->time = 0.0;
  universe->iterations = 0;
  universe->temperature = args->temperature;
  universe->mol_nb = args->molecules;
  universe->force_computation_mode = args->numerical;

  /* Initialize the universe size */
  universe->size = cbrt(0.75*(universe->mol_nb/C_AVOGADRO)*(universe->temperature)*C_IDEALGAS/C_PI);

  /* Open the input file */
  if ((input_file = fopen(args->path, "r")) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Count the lines in the input file to get the molecule size */
  while ((c = getc(input_file)) != EOF)
     if (c == '\n')
       ++(universe->mol_size);
   universe->part_nb = (universe->mol_nb)*(universe->mol_size);

  /* Get the file's size */
  fseek(input_file, 0, SEEK_END);
  file_len = ftell(input_file);
  rewind(input_file);
  
  /* Initialize the memory buffer for the file */
  if ((universe->input_file_buffer = (char*)malloc(file_len+1)) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Load it in the buffer, terminate the string */
  if (fread(universe->input_file_buffer, sizeof(char), file_len, input_file) != file_len)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  universe->input_file_buffer[file_len] = '\0';

  /* Close the file, we're done */
  fclose(input_file);

  /* Allocate memory for the particles */
  if ((universe->particle = malloc((universe->part_nb)*sizeof(particle_t))) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Initialize the particle memory */
  memset(universe->particle, 0, (universe->part_nb)*sizeof(particle_t));
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_init(&(universe->particle[i])) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Load the initial state from the input file */
  if (universe_load(universe) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  
  /* Populate the universe with extra molecules */
  if (universe_populate(universe) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Apply initial velocities */
  if (universe_setvelocity(universe) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Initialize the .xyz file pointer */
  if ((universe->output_file_xyz = fopen(args->out_path, "w")) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  return (universe);
}

universe_t *universe_load(universe_t *universe)
{
  size_t i;
  size_t ii;
  int bond_id[7];
  char *tok;
  particle_t *temp;
  
  /* Load the initial state from the input file */
  tok = strtok(universe->input_file_buffer, "\n");
  for (i=0; i<(universe->mol_size); ++i)
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
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
    temp->mass *= 1.66053904020E-27; /* We convert the values from atomic mass units to kg */
    temp->charge *= 1.602176634E-19; /* Same with charge, from e to C */
    if (vec3d_mul(&(temp->pos), &(temp->pos), 1E-12) == NULL) /* We convert the position vector from pm to m */
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));

    /* Set up the bonds */
    for (ii=0; ii<7; ++ii)
    {
      temp->bond_length[ii] *= 1E-12;
      temp->bond_id[ii] = bond_id[ii];
      if (bond_id[ii] < 0)
        temp->bond[ii] = NULL;
      else
      {
        temp->bond[ii] = &(universe->particle[bond_id[ii]]);
      }
    }
    
    tok = strtok(NULL, "\n");
  }
  return (universe);
}

universe_t *universe_populate(universe_t *universe)
{
  size_t i;
  size_t ii;
  size_t iii;
  size_t id_offset;
  vec3d_t pos_offset;
  particle_t *reference;
  particle_t *current;
  
  /* For every molecule */
  for (i=1; i<(universe->mol_nb); ++i) /* i=1 because we already have a molecule loaded at particle[0] */
  { 
    id_offset = (universe->mol_size)*i;
    
    /* Generate a random vector */
    pos_offset.x = rand()-rand();
    pos_offset.y = rand()-rand();
    pos_offset.z = rand()-rand();
    if (vec3d_unit(&pos_offset, &pos_offset) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_POPULATE_FAILURE, __FILE__, __LINE__));
    if (vec3d_mul(&pos_offset, &pos_offset, 0.4*(universe->size)*cos(rand())) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_POPULATE_FAILURE, __FILE__, __LINE__));
    pos_offset.x += 0.4*(universe->size);
    pos_offset.y += 0.4*(universe->size);
    pos_offset.z += 0.4*(universe->size);
    
    /* For every atom in the molecule */
    for (ii=0; ii<(universe->mol_size); ++ii)
    {
      /* Clone the reference atom */
      reference = &(universe->particle[ii]);
      current = &(universe->particle[ii+id_offset]);
      *current = *reference;
      
      /* Displace the atom to the random vector */
      if (vec3d_add(&(current->pos), &(current->pos), &pos_offset) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_POPULATE_FAILURE, __FILE__, __LINE__));
      
      /* For every bond in the displaced atom */
      for (iii=0; iii<7; ++iii)
      {
        if (current->bond_id[iii] < 0)
          current->bond[iii] = NULL;
        else
        {
          current->bond_id[iii] = reference->bond_id[iii] + id_offset;
          current->bond[iii] = &(universe->particle[current->bond_id[iii]]);
        }
      }
    }
  }
    
  return (universe);
}

universe_t *universe_setvelocity(universe_t *universe)
{
  double mass_mol;
  double kinetic_avg;
  double velocity;
  vec3d_t vec;
  size_t i;
  
  /* Get the average kinetic energy from temperature */
  kinetic_avg = (3/2)*(C_IDEALGAS/C_AVOGADRO)*universe->temperature;
  
  /* Get the molecule's total mass */
  mass_mol = 0;
  for (i=0; i<(universe->mol_size); ++i)
    mass_mol += universe->particle[i].mass;
  
  /* Get the molecule's velocity */
  velocity = sqrt(2*kinetic_avg/mass_mol);

  /* For every atom in the universe */
  for (i=0; i<(universe->part_nb); ++i)
  {
    /* Generate a random unit vector */
    vec.x = cos(rand());
    vec.y = cos(rand());
    vec.z = cos(rand());
    if (vec3d_unit(&vec, &vec) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_SETVELOCITY_FAILURE, __FILE__, __LINE__));
    
    /* Apply an average speed from the average kinetic energy */
    if (vec3d_mul(&(universe->particle[i].spd), &vec, velocity) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_SETVELOCITY_FAILURE, __FILE__, __LINE__));
  }
  
  return (universe);
}

void universe_clean(universe_t *universe)
{
  /* Close the file pointers */
  fclose(universe->output_file_xyz);

  /* Free allocated memory */
  free(universe->input_file_buffer);
  free(universe->particle);}

universe_t *universe_iterate(universe_t *universe, const args_t *args)
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

  return (universe);
}

int universe_simulate(universe_t *universe, const args_t *args)
{

  /* While we haven't reached the target time, we iterate the universe */
  while (universe->time < args->max_time)
  {
    /* Print the message indicating which iteration has been rendered */
    printf(TEXT_ITERATION, universe->iterations);

    /* We print the state to the .xyz file */
    if (universe_printstate(universe) == NULL)
      return (retstri(EXIT_FAILURE, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));

    /* And we iterate */
    if (universe_iterate(universe, args) == NULL)
      return (retstri(EXIT_FAILURE, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));

    universe->time += args->timestep;
    ++(universe->iterations);
  }
  return (EXIT_SUCCESS);
}

universe_t *universe_printstate(universe_t *universe)
{
  size_t i;

  /* Print in the .xyz */
  fprintf(universe->output_file_xyz, "%ld\n%ld\n", universe->part_nb, universe->iterations);
  for (i=0; i<(universe->part_nb); ++i)
  {
    fprintf(universe->output_file_xyz,
            "%s\t%.15lf\t%.15lf\t%.15lf\n",
            universe->particle[i].element,
            universe->particle[i].pos.x*1E10,
            universe->particle[i].pos.y*1E10,
            universe->particle[i].pos.z*1E10);
  }
  return (universe);
}
