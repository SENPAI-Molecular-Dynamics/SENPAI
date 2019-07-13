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
  int32_t c;               /* Used as a character buffer for getc */
  size_t i;                /* Iterator */
  size_t input_file_len;   /* Size of the input file (bytes) */
  char *input_file_buffer; /* A memory copy of the input file */

  /* Initialize the variables */
  universe->meta_name = UNIVERSE_META_NAME_DEFAULT;
  universe->meta_author = UNIVERSE_META_AUTHOR_DEFAULT;
  universe->meta_comment = UNIVERSE_META_COMMENT_DEFAULT;
  universe->sys_size = 0;
  universe->copy_nb = args->copies;
  universe->part_nb = 0;
  universe->time = 0.0;
  universe->temperature = args->temperature;
  universe->pressure = args->pressure;
  universe->iterations = 0;

  /* Open the output file */
  if ((universe->output_file = fopen(args->out_path, "w")) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Open the input file */
  if ((universe->input_file = fopen(args->path, "r")) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Count the lines in the input file to get the molecule size */
  while ((c = getc(universe->input_file)) != EOF)
     if (c == '\n')
       ++(universe->sys_size);
   universe->part_nb = (universe->copy_nb)*(universe->sys_size);

  /* Get the file's size */
  fseek(universe->input_file, 0, SEEK_END);
  input_file_len = ftell(universe->input_file);
  rewind(universe->input_file);
  
  /* Initialize the memory buffer for the file */
  if ((input_file_buffer = (char*)malloc(input_file_len+1)) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Load it in the buffer, terminate the string */
  if (fread(input_file_buffer, sizeof(char), input_file_len, universe->input_file) != input_file_len)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  input_file_buffer[input_file_len] = '\0';

  /* Allocate memory for the particles */
  if ((universe->particle = malloc((universe->part_nb)*sizeof(particle_t))) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Initialize the particle memory */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_init(&(universe->particle[i])) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Load the initial state from the input file */
  if (universe_load(universe, input_file_buffer) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  
  /* Free the buffer, we're done */
  free(input_file_buffer);

  /* Initialize the universe size */
  universe->size = cbrt(C_BOLTZMANN*(universe->part_nb)*(universe->temperature)/(args->pressure));
  
  /* Populate the universe with extra molecules */
  if (universe_populate(universe) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Apply initial velocities */
  if (universe_setvelocity(universe) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  return (universe);
}

universe_t *universe_load(universe_t *universe, char *input_file_buffer)
{
  size_t i;
  size_t ii;
  int bond_id[7];
  char *tok;
  particle_t *temp;
  
  /* Load the initial state from the input file */
  tok = strtok(input_file_buffer, "\n");
  for (i=0; i<(universe->sys_size); ++i)
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
  size_t i;              /* Iterator */
  size_t ii;             /* Iterator */
  size_t iii;            /* Iterator */
  size_t id_offset;      /* ID offset in the (universe.particle) array */
  vec3d_t pos_offset;    /* Physical position offset */
  particle_t *reference; /* Reference particle */
  particle_t *current;   /* Cloned particle */
  
  /* For every molecule */
  for (i=1; i<(universe->copy_nb); ++i) /* i=1 because we already have a molecule loaded at particle[0] */
  { 
    id_offset = (universe->sys_size)*i;

    /* Generate a random vector */
    vec3d_marsaglia(&pos_offset);
    vec3d_mul(&pos_offset, &pos_offset, (0.1*(universe->size)) + (0.8*(universe->size)*cos(rand())));
    
    /* For every atom in the molecule */
    for (ii=0; ii<(universe->sys_size); ++ii)
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

  /* Enforce the PBC */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_enforce_pbc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_POPULATE_FAILURE, __FILE__, __LINE__));
    
  return (universe);
}

/* Apply a velocity to all the system's particles from the average kinetic energy */
universe_t *universe_setvelocity(universe_t *universe)
{
  size_t i;        /* Iterator */
  double mass_mol; /* Mass of a loaded system's */
  double velocity; /* Average velocity calculated */
  vec3d_t vec;     /* Random vector */

  /* Get the molecular mass */
  mass_mol = 0;
  for (i=0; i<(universe->sys_size); ++i)
    mass_mol += model_mass(universe->particle[i].element);

  /* Get the average velocity */
  velocity = sqrt(3*C_BOLTZMANN*(universe->temperature)/mass_mol);

  /* For every atom in the universe */
  for (i=0; i<(universe->part_nb); ++i)
  {
    /* Apply the velocity in a random direction */
    vec3d_marsaglia(&vec);
    vec3d_mul(&(universe->particle[i].vel), &vec, velocity);
  }

  return (universe);
}

void universe_clean(universe_t *universe)
{
  /* Close the file pointers */
  fclose(universe->output_file);
  fclose(universe->input_file);

  /* Free allocated memory */
  free(universe->particle);
}

/* Main loop of the simulator. Iterates until the target time is reached */
universe_t *universe_simulate(universe_t *universe, const args_t *args)
{
  uint64_t frame_nb; /* Used for frameskipping */
  double energy;     /* The system's energy */

  /* Get the system's initial energy */
  if (universe_energy_total(universe, &energy) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));

  /* Print some useful information */
  printf(TEXT_INFO_NAME, universe->meta_name);
  printf(TEXT_INFO_AUTHOR, universe->meta_author);
  printf(TEXT_INFO_COMMENT, universe->meta_comment);
  printf(TEXT_INFO_SYS_SIZE, universe->sys_size);
  printf(TEXT_INFO_SYS_COPIES, universe->copy_nb);
  printf(TEXT_INFO_ATOMS, universe->part_nb);
  printf(TEXT_INFO_TEMPERATURE, universe->temperature);
  printf(TEXT_INFO_PRESSURE, args->pressure/1E2);
  printf(TEXT_INFO_TOTAL_ENERGY, energy*1E12);
  printf(TEXT_INFO_UNIVERSE_SIZE, universe->size*1E12);
  printf(TEXT_INFO_SIMULATION_TIME, args->max_time*1E9);
  printf(TEXT_INFO_TIMESTEP, args->timestep*1E15);
  printf(TEXT_INFO_FRAMESKIP, args->frameskip);
  printf(TEXT_INFO_ITERATIONS, (long)ceil(args->max_time/args->timestep));

  /* Tell the user the simulation is starting */
  puts(TEXT_SIMSTART);

  /* While we haven't reached the target time, we iterate the universe */
  frame_nb = 0;
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

universe_t *universe_iterate(universe_t *universe, const args_t *args)
{
  size_t i; /* Iterator */

  /* We update the position vector first, as part of the Velocity-Verley integration */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_pos(universe, args, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* We enforce the periodic boundary conditions */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_enforce_pbc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* Update the force vectors */
  /* By numerically differentiating the potential energy... */
  if (args->numerical == MODE_NUMERICAL)
  {
    for (i=0; i<(universe->part_nb); ++i)
      if (particle_update_frc_numerical(universe, i) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
  }
  
  /* Or analytically solving for force */
  else
  {
    for (i=0; i<(universe->part_nb); ++i)
      if (particle_update_frc_analytical(universe, i) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
  }

  /* Update the acceleration vectors */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_acc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* Update the speed vectors */
  for (i=0; i<(universe->part_nb); ++i)
    if (particle_update_vel(universe, args, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  return (universe);
}

/* Print the system's state to the .xyz file */
universe_t *universe_printstate(universe_t *universe)
{
  size_t i; /* Iterator */

  /* Print in the .xyz */
  fprintf(universe->output_file, "%ld\n%ld\n", universe->part_nb, universe->iterations);
  for (i=0; i<(universe->part_nb); ++i)
  {
    fprintf(universe->output_file,
            "%s\t%.15lf\t%.15lf\t%.15lf\n",
            model_symbol(universe->particle[i].element),
            universe->particle[i].pos.x*1E10,
            universe->particle[i].pos.y*1E10,
            universe->particle[i].pos.z*1E10);
  }
  return (universe);
}

/* Compute the system's total kinetic energy */
universe_t *universe_energy_kinetic(universe_t *universe, double *energy)
{
  size_t i;   /* Iterator */
  double vel; /* Particle velocity (m/s) */

  *energy = 0.0;
  for (i=0; i<(universe->part_nb); ++i)
  {
    vel = vec3d_mag(&(universe->particle[i].vel));
    *energy += 0.5*POW2(vel)*model_mass(universe->particle[i].element);
  }

  return (universe);
}

/* Compute the system's total potential energy */
universe_t *universe_energy_potential(universe_t *universe, double *energy)
{
  size_t i;         /* Iterator */
  double potential; /* Total potential energy */

  *energy = 0.0;
  for (i=0; i<(universe->part_nb); ++i)
  {
    if (potential_total(&potential, universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ENERGY_POTENTIAL_FAILURE, __FILE__, __LINE__));
    *energy += potential;
  }

  return (universe);
}

/* Compute the total system energy */
universe_t *universe_energy_total(universe_t *universe, double *energy)
{
  double kinetic;   /* Total kinetic energy */
  double potential; /* Total potential energy */

  /* Get the kinetic energy */
  if (universe_energy_kinetic(universe, &kinetic) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ENERGY_TOTAL_FAILURE, __FILE__, __LINE__));

  /* Get the potential energy */
  if (universe_energy_potential(universe, &potential) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ENERGY_TOTAL_FAILURE, __FILE__, __LINE__));

  /* Get the total energy */
  *energy = kinetic + potential;
  return (universe);
}

/* Apply random transformations to lower the system's potential energy */
universe_t *universe_montecarlo(universe_t *universe)
{
  double potential;      /* Pre-transformation potential energy */
  double potential_new;  /* Post-transformation potential energy */
  double pos_offset_mag; /* Magnitude of the position offset vector */
  uint64_t part_id;      /* ID of the particle currently being offset */
  uint64_t tries;        /* How many times we tried to offset the particle */
  vec3d_t pos_offset;    /* Position offset vector */
  vec3d_t pos_backup;    /* A backup of the pre-transformation position vector */

  /* For each particle in the universe */
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
        pos_offset_mag *= 0.1; /* Refine the random displacement */
      }

      /* Back up the coordinates */
      pos_backup = universe->particle[part_id].pos;

      /* Apply a random transformation */
      vec3d_marsaglia(&pos_offset);
      vec3d_mul(&pos_offset, &pos_offset, pos_offset_mag);
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