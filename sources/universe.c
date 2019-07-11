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
#include "model.h"
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

  /* Initialize the universe size */
  universe->size = cbrt(C_BOLTZMANN*(universe->part_nb)*(universe->temperature)/(args->pressure));
  
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
               "%hhu,%lf,%lf,%lf,%d,%d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
               &(temp->element),
               &(temp->charge),
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
               &(temp->pos.x),
               &(temp->pos.y),
               &(temp->pos.z)) < 0)
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));

    /* Scale from elementary charges to Coulombs */
    temp->charge *= 1.602176634E-19;

    /* Scale from Angstroms to metres */
    temp->sigma *= 1E-10;
    temp->pos.x *= 1E-10;
    temp->pos.y *= 1E-10;
    temp->pos.z *= 1E-10;

    /* Set up the bonds */
    for (ii=0; ii<7; ++ii)
    {
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
    pos_offset.x = -0.3*(universe->size) + fmod(rand(), 0.6*(universe->size));
    pos_offset.y = -0.3*(universe->size) + fmod(rand(), 0.6*(universe->size));
    pos_offset.z = -0.3*(universe->size) + fmod(rand(), 0.6*(universe->size));
    if (vec3d_unit(&pos_offset, &pos_offset) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_POPULATE_FAILURE, __FILE__, __LINE__));

    /* Multiply the vector by a random value */
    vec3d_mul(&pos_offset, &pos_offset, (0.1*(universe->size)) + (0.8*(universe->size)*cos(rand())));
    
    /* For every atom in the molecule */
    for (ii=0; ii<(universe->mol_size); ++ii)
    {
      /* Clone the reference atom */
      reference = &(universe->particle[ii]);
      current = &(universe->particle[ii+id_offset]);
      *current = *reference;
      
      /* Displace the atom to the random vector */
      vec3d_add(&(current->pos), &(current->pos), &pos_offset);
      
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
    mass_mol += model_mass(universe->particle[i].element);

  /* Get the molecule's velocity */
  velocity = sqrt(3*C_BOLTZMANN*(universe->temperature)/mass_mol);

  /* For every atom in the universe */
  for (i=0; i<(universe->part_nb); ++i)
  {
    /* Generate a random vector */
    vec3d_marsaglia(&vec);

    /* Apply an average velocity from the average kinetic energy */
    vec3d_mul(&(universe->particle[i].spd), &vec, velocity);
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

  /* We enforce the periodic boundary conditions */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_enforce_pbc(universe, i) == NULL)
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
  uint64_t frame_nb;
  double energy;

  frame_nb = 0;

  if (universe_energy_total(universe, &energy) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));

  /* Print some useful information */
  printf("Molecules..............%ld\n", universe->mol_nb);
  printf("Particles..............%ld\n", universe->part_nb);
  printf("Temperature............%lf K\n", universe->temperature);
  printf("Pressure...............%lf hPa\n", args->pressure/1E2);
  printf("Total system energy....%lf pJ\n", energy*1E12);
  printf("Universe size  ........%lf pm\n", universe->size*1E12);
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

    /* Iterate */
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
            model_symbol(universe->particle[i].element),
            universe->particle[i].pos.x*1E10,
            universe->particle[i].pos.y*1E10,
            universe->particle[i].pos.z*1E10);
  }
  return (universe);
}
universe_t *universe_energy_kinetic(universe_t *universe, double *energy)
{
  size_t i;
  double vel;

  *energy = 0.0;
  for (i=0; i<(universe->part_nb); ++i)
  {
    vel = vec3d_mag(&(universe->particle[i].spd));
    *energy += 0.5*POW2(vel)*model_mass(universe->particle[i].element);
  }

  return (universe);
}

universe_t *universe_energy_potential(universe_t *universe, double *energy)
{
  size_t i;
  double potential;

  *energy = 0.0;
  for (i=0; i<(universe->part_nb); ++i)
  {
    if (potential_total(&potential, universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ENERGY_POTENTIAL_FAILURE, __FILE__, __LINE__));
    *energy += potential;
  }

  return (universe);
}

universe_t *universe_energy_total(universe_t *universe, double *energy)
{
  double kinetic;
  double potential;

  if (universe_energy_kinetic(universe, &kinetic) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ENERGY_TOTAL_FAILURE, __FILE__, __LINE__));
  if (universe_energy_potential(universe, &potential) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ENERGY_TOTAL_FAILURE, __FILE__, __LINE__));

  *energy = kinetic + potential;
  return (universe);
}

universe_t *universe_montecarlo(universe_t *universe)
{
  double potential;
  double potential_new;
  double pos_offset_mag;
  uint64_t part_id;
  uint64_t tries;
  vec3d_t pos_offset;
  vec3d_t pos_backup;

  for (part_id=0; part_id<(universe->part_nb); ++part_id)
  {
    pos_offset_mag = 1E-9;

    /* Get the system's total potential energy */
    if (universe_energy_potential(universe, &potential) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ENERGY_TOTAL_FAILURE, __FILE__, __LINE__));

    tries = 0;
    while (1)
    {
      if (tries < 50)
        ++tries;
      else
      {
        tries = 0;
        pos_offset_mag *= 0.5; /* Refine the random displacement */
      }

      /* Back up the coordinates */
      pos_backup = universe->particle[part_id].pos;

      /* Generate a random transformation */
      vec3d_marsaglia(&pos_offset);
      vec3d_mul(&pos_offset, &pos_offset, pos_offset_mag);

      /* Apply the random transformation */
      vec3d_add(&(universe->particle[part_id].pos), &(universe->particle[part_id].pos), &pos_offset);

      /* Enforce the PBC */
      if (particle_enforce_pbc(universe, part_id) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_MONTECARLO_FAILURE, __FILE__, __LINE__));

      /* Get the system's total potential energy */
      if (universe_energy_potential(universe, &potential_new) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_ENERGY_TOTAL_FAILURE, __FILE__, __LINE__));

      /* If the transformation decreases the potential, we're done */
      if (potential_new < potential)
        break;

      /* Otherwise, discard the transformation */
      universe->particle[part_id].pos = pos_backup;
    }
  }

  return (universe);
}