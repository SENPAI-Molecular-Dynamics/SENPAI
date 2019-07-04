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
  universe->size = cbrt(0.75*C_BOLTZMANN*(universe->mol_nb)*(universe->temperature)/((args->pressure)*M_PI));

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
               "%2s,%lf,%lf,%lf,%lf,%lf,%d,%d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
               temp->element,
               &(temp->mass),
               &(temp->charge),
               &(temp->angle),
               &(temp->epsilon),
               &(temp->sigma),
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
    temp->sigma *= 1E-10; /* Scale from Angstroms to metres */

    if (vec3d_mul(&(temp->pos), &(temp->pos), 1E-10) == NULL) /* We convert the position vector from angstroms to m */
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));

    /* Set up the bonds */
    for (ii=0; ii<7; ++ii)
    {
      temp->bond_length[ii] *= 1E-10; /* Scale from angstrom */
      temp->bond_id[ii] = bond_id[ii];

      if (bond_id[ii] < 0)
        temp->bond[ii] = NULL;
      else
        temp->bond[ii] = &(universe->particle[bond_id[ii]]);
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

    /* Generate a random unit vector */
    if (vec3d_marsaglia(&pos_offset) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_POPULATE_FAILURE, __FILE__, __LINE__));

    /* Multiply the vector by a random value */
    if (vec3d_mul(&pos_offset, &pos_offset, (0.1*(universe->size)) + (0.8*(universe->size)*cos(rand()))) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_POPULATE_FAILURE, __FILE__, __LINE__));
    
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
  double velocity;
  vec3d_t vec;
  size_t i;

  /* Get the molecule's total mass */
  mass_mol = 0;
  for (i=0; i<(universe->mol_size); ++i)
    mass_mol += universe->particle[i].mass;

  /* Get the molecule's velocity */
  velocity = sqrt(3*C_BOLTZMANN*(universe->temperature)/mass_mol);

  /* For every atom in the universe */
  for (i=0; i<(universe->part_nb); ++i)
  {
    if (vec3d_marsaglia(&vec) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_SETVELOCITY_FAILURE, __FILE__, __LINE__));

    /* Apply an average velocity from the average kinetic energy */
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

universe_t *universe_simulate(universe_t *universe, const args_t *args)
{
  double energy;
  uint8_t err_flag;
  uint64_t frame_nb;

  err_flag = 0;
  frame_nb = 0;
  energy = universe_energy(universe, &err_flag);

  if (err_flag)
    return (retstr(NULL, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));

  /* Print some useful information */
  printf("Molecules..............%ld\n", universe->mol_nb);
  printf("Particles..............%ld\n", universe->part_nb);
  printf("Temperature............%lf K\n", universe->temperature);
  printf("Pressure...............%lf hPa\n", args->pressure/1E2);
  printf("Total system energy....%lf pJ\n", energy*1E12);
  printf("Universe radius........%lf pm\n", universe->size*1E12);
  printf("Simulation time........%lf ns\n", args->max_time*1E9);
  printf("Timestep...............%lf fs\n", args->timestep*1E15);
  printf("Frameskip..............%ld\n", args->frameskip);
  printf("Iterations.............%ld\n\n", (long)ceil(args->max_time/args->timestep));

  puts(TEXT_SIMSTART);
  /* While we haven't reached the target time, we iterate the universe */
  while (universe->time < args->max_time)
  {
    /* Print the state to the .xyz file, if required */
    if (!frame_nb)
    {
      if (universe_printstate(universe) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));
      frame_nb = (args->frameskip);
    }
    else
      --frame_nb;

    /* And we iterate */
    if (universe_iterate(universe, args) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));

    universe->time += args->timestep;
    ++(universe->iterations);
  }
  puts(TEXT_SIMEND);

  return (universe);
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

double universe_energy(universe_t *universe, uint8_t *err_flag)
{
  size_t i;
  double vel;
  double kinetic;
  double potential;

  *err_flag = 0;
  kinetic = 0.0;
  potential = 0.0;

  /* Get total potential energy */
  for (i=0; i<(universe->part_nb); ++i)
  {
    potential += potential_total(universe, i, err_flag);
    if (*err_flag)
      return (retstrf(0.0, TEXT_UNIVERSE_ENERGY_FAILURE, __FILE__, __LINE__));
  }

  /* Get total kinetic energy */
  for (i=0; i<(universe->part_nb); ++i)
  {
    if ((vel = vec3d_mag(&(universe->particle[i].spd))) < 0.0)
      return (retstrf(0.0, TEXT_UNIVERSE_ENERGY_FAILURE, __FILE__, __LINE__));
    kinetic += 0.5*(universe->particle[i].mass)*POW2(vel);
  }

  return (kinetic+potential);
}