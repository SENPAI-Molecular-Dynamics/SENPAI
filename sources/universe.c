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
#include <pthread.h>

#include "config.h"
#include "args.h"
#include "model.h"
#include "text.h"
#include "vec3d.h"
#include "util.h"
#include "universe.h"
#include "potential.h"

universe_t *universe_init(universe_t *universe, const args_t *args)
{
  size_t i;                /* Iterator */
  size_t input_file_len;   /* Size of the input file (bytes) */
  char *input_file_buffer; /* A memory copy of the input file */
  double universe_mass;    /* Total mass of the universe */

  /* Initialize the structure variables */
  universe->meta_name = NULL;
  universe->meta_author = NULL;
  universe->meta_comment = NULL;
  universe->ref_atom_nb = 0;
  universe->ref_bond_nb = 0;
  universe->copy_nb = args->copies;
  universe->atom_nb = 0;
  universe->size = 0.0;
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

  /* Load the initial state from the input file */
  if (universe_load(universe, input_file_buffer) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Free the input file buffer, we're done */
  free(input_file_buffer);

  /* Initialize the atom number */
  universe->atom_nb = (universe->ref_atom_nb) * (universe->copy_nb);

  /* Allocate memory for the atoms */
  if ((universe->atom = malloc (sizeof(atom_t)*(universe->atom_nb))) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Initialize the atom memory */
  for (i=0; i<(universe->atom_nb); ++i)
    atom_init(&(universe->atom[i]));

  /* Compute the universe's size from the system density */
  /* size = cbrt(universe_mass / system_density) */
  universe_mass = 0.0;
  for (i=0; i<(universe->ref_atom_nb); ++i)
    universe_mass += (args->copies)*model_mass(universe->ref_atom[i].element);
  universe->size = cbrt((universe_mass) / (args->density));

  /* Populate the universe with extra molecules */
  if (universe_populate(universe) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Enforce the PBC */
  for (i=0; i<(universe->atom_nb); ++i)
    if (atom_enforce_pbc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  /* Apply initial velocities */
  if (universe_setvelocity(universe) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));

  return (universe);
}

universe_t *universe_load(universe_t *universe, char *input_file_buffer)
{
  size_t i;
  char *tok;
  /* Used when reading the bond block */
  uint64_t *a1;
  uint64_t *a2;
  double *bond_strength;
  uint8_t *bond_index;
  uint64_t first;
  uint64_t second;

  /* Get the name line and load the system's name from it */
  tok = strtok(input_file_buffer, "\n");
  if ((universe->meta_name = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  strcpy(universe->meta_name, tok);

  /* Get the author line and load the author's name from it */
  tok = strtok(NULL, "\n");
  if ((universe->meta_author = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  strcpy(universe->meta_author, tok);

  /* Get the comment line and load the author's comment from it */
  tok = strtok(NULL, "\n");
  if ((universe->meta_comment = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  strcpy(universe->meta_comment, tok);

  /* Get the count line, and the atom/bond nb from it  */
  tok = strtok(NULL, "\n");
  sscanf(tok, "%ld %ld", &(universe->ref_atom_nb), &(universe->ref_bond_nb));

  /* Allocate memory for the atoms */
  if ((universe->ref_atom = malloc (sizeof(atom_t)*(universe->ref_atom_nb))) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));

  /* Initialize the atom memory */
  for (i=0; i<(universe->ref_atom_nb); ++i)
    atom_init(&(universe->ref_atom[i]));

  /* Read the atom block */
  for (i=0; i<(universe->ref_atom_nb); ++i)
  {
    tok = strtok(NULL, "\n");
    sscanf(tok, "%lf %lf %lf %hhu %lf %lf %lf",
           &(universe->ref_atom[i].pos.x),
           &(universe->ref_atom[i].pos.y),
           &(universe->ref_atom[i].pos.z),
           &(universe->ref_atom[i].element),
           &(universe->ref_atom[i].charge),
           &(universe->ref_atom[i].epsilon),
           &(universe->ref_atom[i].sigma));

    /* Scale the atom's position vector from Å to m */
    vec3d_mul(&(universe->ref_atom[i].pos), &(universe->ref_atom[i].pos), 1E-10);

    /* Scale the atom's charge from C.e-1 to C */
    universe->ref_atom[i].charge *= C_ELEMCHARGE;
  }

  /* Allocate memory for the temporary bond information storage */
  if ((a1 = malloc(sizeof(uint64_t) * (universe->ref_bond_nb))) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  if ((a2 = malloc(sizeof(uint64_t) * (universe->ref_bond_nb))) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  if ((bond_strength = malloc(sizeof(double) * (universe->ref_bond_nb))) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  /* Zero the allocated memory */
  if ((bond_index = calloc(universe->ref_atom_nb, sizeof(uint8_t))) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));

  /* Read the bond block and get the number of bonds for each atom */
  for (i=0; i<(universe->ref_bond_nb); ++i)
  {
    tok = strtok(NULL, "\n");
    sscanf(tok, "%ld %ld %lf", &(a1[i]), &(a2[i]), &(bond_strength[i]));
    ++(universe->ref_atom[a1[i] - 1].bond_nb);
    ++(universe->ref_atom[a2[i] - 1].bond_nb);
  }

  /* Allocate memory for the bond information */
  for (i=0; i<(universe->ref_atom_nb); ++i)
  {
    /* For the bond nodes */
    if ((universe->ref_atom[i].bond = malloc(sizeof(uint64_t) * universe->ref_atom[i].bond_nb)) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));

    /* For the bond strengths */
    if ((universe->ref_atom[i].bond_strength = malloc(sizeof(double) * universe->ref_atom[i].bond_nb)) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }

  /* Load the bond information */
  for (i=0; i<(universe->ref_bond_nb); ++i)
  {
    first = a1[i] - 1;
    second = a2[i] - 1;

    universe->ref_atom[first].bond[bond_index[first]] = second;
    universe->ref_atom[first].bond_strength[bond_index[first]] = bond_strength[i];

    universe->ref_atom[second].bond[bond_index[second]] = first;
    universe->ref_atom[second].bond_strength[bond_index[second]] = bond_strength[i];

    ++bond_index[first];
    ++bond_index[second];
  }

  /* Free the temporary bond information storage */
  free(a1);
  free(a2);
  free(bond_strength);
  free(bond_index);

  return (universe);
}

universe_t *universe_populate(universe_t *universe)
{
  size_t i;
  size_t ii;
  size_t iii;
  vec3d_t pos_offset;
  atom_t *reference;
  atom_t *duplicate;

  for (i=0; i<(universe->copy_nb); ++i)
  {
    /* Generate a random position vector to load the system at */
    vec3d_marsaglia(&pos_offset);
    vec3d_mul(&pos_offset, &pos_offset, (1-UNIVERSE_POPULATE_MIN_DIST)*(universe->size)*cos(rand()) + UNIVERSE_POPULATE_MIN_DIST*(universe->size));

    /* Load each atom from the reference system into the universe */
    for (ii=0; ii<(universe->ref_atom_nb); ++ii)
    {
      /* Just shortcuts, they make the code cleaner */
      reference = &(universe->ref_atom[ii]);
      duplicate = &(universe->atom[(i*(universe->ref_atom_nb)) + ii]);

      duplicate->element = reference->element;
      duplicate->charge = reference->charge;
      duplicate->epsilon = reference->epsilon;
      duplicate->sigma = reference->sigma;

      duplicate->bond_nb = reference->bond_nb;

      duplicate->vel.x = reference->vel.x;
      duplicate->vel.y = reference->vel.y;
      duplicate->vel.z = reference->vel.z;

      duplicate->acc.x = reference->acc.x;
      duplicate->acc.y = reference->acc.y;
      duplicate->acc.z = reference->acc.z;

      duplicate->frc.x = reference->frc.x;
      duplicate->frc.y = reference->frc.y;
      duplicate->frc.z = reference->frc.z;

      /* Load the atom's location */
      vec3d_add(&(duplicate->pos), &(reference->pos), &pos_offset);

      /* Allocate memory for the bond information */
      if ((duplicate->bond = malloc(sizeof(uint64_t)*(reference->bond_nb))) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_POPULATE_FAILURE, __FILE__, __LINE__));
      if ((duplicate->bond_strength = malloc(sizeof(double)*(reference->bond_nb))) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_POPULATE_FAILURE, __FILE__, __LINE__));

      /* Load the bond information */
      for (iii=0; iii<(duplicate->bond_nb); ++iii)
      {
        duplicate->bond[iii] = reference->bond[iii] + (i*(universe->ref_atom_nb));
        duplicate->bond_strength[iii] = reference->bond_strength[iii];
      }
    }
  }

  return (universe);
}

/* Apply a velocity to all the system's atoms from the average kinetic energy */
universe_t *universe_setvelocity(universe_t *universe)
{
  size_t i;        /* Iterator */
  double mass_mol; /* Mass of a loaded system's */
  double velocity; /* Average velocity calculated */
  vec3d_t vec;     /* Random vector */

  /* Get the molecular mass */
  mass_mol = 0;
  for (i=0; i<(universe->ref_atom_nb); ++i)
    mass_mol += model_mass(universe->ref_atom[i].element);

  /* Get the average velocity */
  velocity = sqrt(3*C_BOLTZMANN*(universe->temperature)/mass_mol);

  /* For every atom in the universe */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    /* Apply the velocity in a random direction */
    vec3d_marsaglia(&vec);
    vec3d_mul(&(universe->atom[i].vel), &vec, velocity);
  }

  return (universe);
}

void universe_clean(universe_t *universe)
{
  size_t i;

  /* Clean each atom in the reference system */
  for (i=0; i<(universe->ref_atom_nb); ++i)
    atom_clean(&(universe->ref_atom[i]));

  /* Clean each atom in the universe */
  for (i=0; i<(universe->atom_nb); ++i)
    atom_clean(&(universe->atom[i]));

  /* Close the file pointers */
  fclose(universe->output_file);
  fclose(universe->input_file);

  /* Free allocated memory */
  free(universe->meta_name);
  free(universe->meta_author);
  free(universe->meta_comment);
  free(universe->ref_atom);
  free(universe->atom);
}


struct thread_args_struct {
    int thread_id;
    const args_t *args;
    universe_t *universe;
};


void* thread_universe_iterate(void *var)
{
  struct thread_args_struct *t_args = (struct thread_args_struct*)var;
	int thread_id = (*t_args).thread_id;
  const args_t *args = (*t_args).args;
  universe_t *universe = (*t_args).universe;


  printf("[Thread %d]Am initializat argumentele si universul\n", thread_id);

  uint64_t frame_nb = 0; /* Used for frameskipping */

  int N = universe->atom_nb;
  int P = 4;

  while (universe->time < args->max_time)
  {
    if (thread_id == 0) {
      /* Print the state to the .xyz file, if required */
      if (!frame_nb)
      {
        if (universe_printstate(universe) == NULL)
          return (retstri(EXIT_FAILURE, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));
        frame_nb = (args->frameskip);
      }
      else
        --frame_nb;
    }

    // /* Iterate */
    // if (universe_iterate(universe, args) == NULL)
    //   return (retstri(EXIT_FAILURE, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));
    size_t i; /* Iterator */

    if (thread_id == 0) {
      /* We update the position vector first, as part of the Velocity-Verley integration */
      for (i=0; i<(universe->atom_nb); ++i)
        if (atom_update_pos(universe, args, i) == NULL)
          return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
    }
    /* We enforce the periodic boundary conditions */
    if (thread_id == 0) {
      for (i=0; i<(universe->atom_nb); ++i)
        atom_enforce_pbc(universe, i);
        // if (atom_enforce_pbc(universe, i) == NULL)
        //   return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
    }
    /* Update the force vectors */
    /* By numerically differentiating the potential energy... */
    if (args->numerical == MODE_NUMERICAL)
    {
      // for (i=0; i<(universe->atom_nb); ++i)
      for(i = thread_id * N / P; i < (thread_id + 1) * N / P - 1; i++) 
        atom_update_frc_numerical(universe, i);
        // if (atom_update_frc_numerical(universe, i) == NULL)
        //   return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
    }

    /* Or analytically solving for force */
    else
    {
      // for (i=0; i<(universe->atom_nb); ++i)
      for(i = thread_id * N / P; i < (thread_id + 1) * N / P - 1; i++)
        atom_update_frc_analytical(universe, i);
        // if (atom_update_frc_analytical(universe, i) == NULL)
        //   return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
    }

    /* Update the acceleration vectors */
    // for (i=0; i<(universe->atom_nb); ++i)
    for(i = thread_id * N / P; i < (thread_id + 1) * N / P - 1; i++)
      atom_update_acc(universe, i);
      // if (atom_update_acc(universe, i) == NULL)
      //   return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

    /* Update the speed vectors */
    // for (i=0; i<(universe->atom_nb); ++i)
    for(i = thread_id * N / P; i < (thread_id + 1) * N / P - 1; i++)
      atom_update_vel(universe, args, i);
      // if (atom_update_vel(universe, args, i) == NULL)
      //   return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));


    universe->time += args->timestep;
    ++(universe->iterations);
  }
}


/* Main loop of the simulator. Iterates until the target time is reached */
int universe_simulate(universe_t *universe, const args_t *args)
{
  uint64_t frame_nb; /* Used for frameskipping */

  /* Tell the user the simulation is starting */
  puts(TEXT_SIMSTART);

  /* While we haven't reached the target time, we iterate the universe */
  frame_nb = 0;


  // TODO create threads here
  int P = 4;
	int i;

	pthread_t tid[P];
	int thread_id[P];
	// for(i = 0;i < P; i++)
		// thread_id[i] = i;

	for(i = 0; i < P; i++) {
    struct thread_args_struct *aux;
    aux = malloc(sizeof(struct thread_args_struct));
    (*aux).thread_id = i;
    (*aux).args = args;
    (*aux).universe = universe;
    printf("Am creat threadul %d\n", i);
		pthread_create(&(tid[i]), NULL, thread_universe_iterate, (aux));
	}

  // TODO destroy threads here

	for(i = 0; i < P; i++) {
    printf("Am distrus threadul %d\n", i);
		pthread_join(tid[i], NULL);
	}

  /* End of simulation */
  puts(TEXT_SIMEND);
  universe_clean(universe);

  return (EXIT_SUCCESS);
}

universe_t *universe_iterate(universe_t *universe, const args_t *args)
{
  size_t i; /* Iterator */

  /* We update the position vector first, as part of the Velocity-Verley integration */
  for (i=0; i<(universe->atom_nb); ++i)
    if (atom_update_pos(universe, args, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* We enforce the periodic boundary conditions */

  for (i=0; i<(universe->atom_nb); ++i)
    if (atom_enforce_pbc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* Update the force vectors */
  /* By numerically differentiating the potential energy... */
  if (args->numerical == MODE_NUMERICAL)
  {
    for (i=0; i<(universe->atom_nb); ++i)
      if (atom_update_frc_numerical(universe, i) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
  }

  /* Or analytically solving for force */
  else
  {
    for (i=0; i<(universe->atom_nb); ++i)
      if (atom_update_frc_analytical(universe, i) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
  }

  /* Update the acceleration vectors */
  for (i=0; i<(universe->atom_nb); ++i)
    if (atom_update_acc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  /* Update the speed vectors */
  for (i=0; i<(universe->atom_nb); ++i)
    if (atom_update_vel(universe, args, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));

  return (universe);
}

/* Print the system's state to the .xyz file */
universe_t *universe_printstate(universe_t *universe)
{
  size_t i; /* Iterator */

  /* Print in the .xyz */
  fprintf(universe->output_file, "%ld\n%ld\n", universe->atom_nb, universe->iterations);
  for (i=0; i<(universe->atom_nb); ++i)
  {
    fprintf(universe->output_file,
            "%s\t%lf\t%lf\t%lf\n",
            model_symbol(universe->atom[i].element),
            universe->atom[i].pos.x*1E10,
            universe->atom[i].pos.y*1E10,
            universe->atom[i].pos.z*1E10);
  }
  return (universe);
}

/* Compute the system's total kinetic energy */
universe_t *universe_energy_kinetic(universe_t *universe, double *energy)
{
  size_t i;   /* Iterator */
  double vel; /* Particle velocity (m/s) */

  *energy = 0.0;
  for (i=0; i<(universe->atom_nb); ++i)
  {
    vel = vec3d_mag(&(universe->atom[i].vel));
    *energy += 0.5*POW2(vel)*model_mass(universe->atom[i].element);
  }

  return (universe);
}

/* Compute the system's total potential energy */
universe_t *universe_energy_potential(universe_t *universe, double *energy)
{
  size_t i;         /* Iterator */
  double potential; /* Total potential energy */

  *energy = 0.0;
  for (i=0; i<(universe->atom_nb); ++i)
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

/* Move the atoms around until a target potential is reached */
universe_t *universe_reducepot(universe_t *universe, args_t *args)
{
  double potential;

  /* Compute and print the current potential */
  if (universe_energy_potential(universe, &potential) == NULL)
    return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
  printf(TEXT_POTENTIAL, potential*1E12);

  /* Print the target potential */
  printf(TEXT_REDUCEPOT, args->reduce_potential*1E12);

  /* PHASE 1 - BRUTEFORCE */
  while (potential > UNIVERSE_REDUCEPOT_COARSE_THRESHOLD * args->reduce_potential)
  {
    if (universe_reducepot_coarse(universe) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));

    /* Update the system's potential energy */
    if (universe_energy_potential(universe, &potential) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
  }

  /* PHASE 2 - GRADIENT DESCENT */
  while (potential > args->reduce_potential)
  {
    if (universe_reducepot_fine(universe) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));

    /* Update the system's potential energy */
    if (universe_energy_potential(universe, &potential) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
  }

  return (universe);
}

/* Apply transformations to lower the system's potential energy (Bruteforcing) */
universe_t *universe_reducepot_coarse(universe_t *universe)
{
  size_t i;
  size_t tries;
  double step_magnitude;
  double pot_pre;
  double pot_post;
  vec3d_t step;
  vec3d_t pos_pre;

  /* For each atom */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    /* Backup the coordinates */
    pos_pre.x = universe->atom[i].pos.x;
    pos_pre.y = universe->atom[i].pos.y;
    pos_pre.z = universe->atom[i].pos.z;

    /* Compute the pre-transformation potential */
    if (universe_energy_total(universe, &pot_pre) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_COARSE_FAILURE, __FILE__, __LINE__));

    /* Until we lower the potential */
    tries = 0;
    step_magnitude = UNIVERSE_REDUCEPOT_COARSE_STEP_MAGNITUDE;
    do
    {
      /* Reset the displacement */
      universe->atom[i].pos = pos_pre;

      /* Compute the displacement magnitude */
      if (tries == UNIVERSE_REDUCEPOT_COARSE_MAX_ATTEMPTS)
      {
        tries = 0;
        step_magnitude *= UNIVERSE_REDUCEPOT_COARSE_MAGNITUDE_MULTIPLIER;
      }
      else
        ++tries;

      /* Compute the displacement */
      vec3d_marsaglia(&step);
      vec3d_mul(&step, &step, step_magnitude);

      /* Apply the displacement */
      vec3d_add(&(universe->atom[i].pos), &(universe->atom[i].pos), &step);

      /* Enforce PBCs */
      if (atom_enforce_pbc(universe, i) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_COARSE_FAILURE, __FILE__, __LINE__));

      /* Compute the post-transformation potential */
      if (universe_energy_total(universe, &pot_post) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_COARSE_FAILURE, __FILE__, __LINE__));
    } while (pot_post > pot_pre);
  }

  return (universe);
}

/* Apply transformations to lower the system's potential energy (Gradient descent) */
universe_t *universe_reducepot_fine(universe_t *universe)
{
  size_t i;
  double step_magnitude;
  double pot_pre;
  double pot_post;
  vec3d_t step;
  vec3d_t pos_pre;

  /* For each atom */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    /* Backup the coordinates */
    pos_pre = universe->atom[i].pos;

    /* Compute the potential gradient with respect to the atom's coordinates (=force) */
    if (atom_update_frc_analytical(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FINE_FAILURE, __FILE__, __LINE__));

    /* "Take a step" toward the local minima*/
    /* The magnitude is derived from the displacement resulting from a constant acceleration */
    /* displacement = acceleration * t^2 * 0.5 */
    /*              = force * t^2 / (2 * mass) */
    /* The direction in which the step is taken is derived from the force vector, since force = -nabla*potential */
    /* Motion is just fancy gradient descent that instead of bleeding potential conserves it as kinetic energy */
    /* Think of this algorithm as a simulation without motion, we're just reaching equilibrium without motion */
    step_magnitude = POW2(UNIVERSE_REDUCEPOT_FINE_TIMESTEP)/(2*model_mass(universe->atom[i].element));
    vec3d_mul(&step, &(universe->atom[i].frc), step_magnitude);

    /* Limit the maximum displacement to 1 Angstrom */
    if (vec3d_mag(&step) > UNIVERSE_REDUCEPOT_FINE_MAX_STEP)
      vec3d_mul(&step, &step, UNIVERSE_REDUCEPOT_FINE_MAX_STEP/vec3d_mag(&step));

    /* Compute the potential before the transformation */
    if (universe_energy_potential(universe, &pot_pre) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FINE_FAILURE, __FILE__, __LINE__));

    /* Apply the transformation */
    vec3d_add(&(universe->atom[i].pos), &(universe->atom[i].pos), &step);

    /* Enforce PBCs */
    if (atom_enforce_pbc(universe, i) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FINE_FAILURE, __FILE__, __LINE__));

    /* Compute the potential after the transformation */
    if (universe_energy_potential(universe, &pot_post) == NULL)
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FINE_FAILURE, __FILE__, __LINE__));

    /* If the potential increased, discard the transformation */
    if (pot_post > pot_pre)
      universe->atom[i].pos = pos_pre;
  }

  return (universe);
}

universe_t *universe_parameters_print(universe_t *universe, const args_t *args)
{
  double potential;    /* The universe's potential energy */

  /* Compute the potential energy */
   if (universe_energy_potential(universe, &potential) == NULL)
     return (retstr(NULL, TEXT_UNIVERSE_PARAMETERS_PRINT_FAILURE, __FILE__, __LINE__));
  
  /* Print some useful information */
  puts(TEXT_INFO_REFERENCE);
  printf(TEXT_INFO_PATH, args->path);
  printf(TEXT_INFO_NAME, universe->meta_name);
  printf(TEXT_INFO_AUTHOR, universe->meta_author);
  printf(TEXT_INFO_COMMENT, universe->meta_comment);
  printf(TEXT_INFO_REF_ATOM_NB, universe->ref_atom_nb);
  printf(TEXT_INFO_REF_BOND_NB, universe->ref_bond_nb);
  printf("\n");
  puts(TEXT_INFO_SIMULATION);
  printf(TEXT_INFO_SYS_COPIES, universe->copy_nb);
  printf(TEXT_INFO_ATOMS, universe->atom_nb);
  printf(TEXT_INFO_TEMPERATURE, universe->temperature);
  printf(TEXT_INFO_PRESSURE, args->pressure/1E2);
  printf(TEXT_INFO_DENSITY, args->density/1E3);
  printf(TEXT_INFO_POTENTIAL_ENERGY, potential*1E12);
  printf(TEXT_INFO_UNIVERSE_SIZE, universe->size*1E12);
  printf(TEXT_INFO_SIMULATION_TIME, args->max_time*1E9);
  printf(TEXT_INFO_TIMESTEP, args->timestep*1E15);
  printf(TEXT_INFO_FRAMESKIP, args->frameskip);
  printf(TEXT_INFO_ITERATIONS, (long)ceil(args->max_time/args->timestep));

  return (universe);
}