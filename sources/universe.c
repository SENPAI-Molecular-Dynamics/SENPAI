/*
 * universe.c
 *
 * Licensed under GPLv3 license
 *
 */

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "config.h"
#include "args.h"
#include "model.h"
#include "text.h"
#include "vec3.h"
#include "util.h"
#include "universe.h"
#include "potential.h"

universe_t *universe_init(universe_t *universe, const args_t *args)
{
  size_t i;                    /* Iterator */
  size_t file_len_model;       /* Size of the model file (bytes) */
  size_t file_len_substrate;   /* Size of the substrate file (bytes) */
  size_t file_len_solvent;     /* Size of the solvent file (bytes) */
  char *file_buffer_model;     /* A memory copy of the model */
  char *file_buffer_substrate; /* A memory copy of the substrate file */
  char *file_buffer_solvent;   /* A memory copy of the solvent file */
  double universe_mass;        /* Total mass of the universe */

  /* Initialize the structure variables */
  universe->file_model = UNIVERSE_FILE_MODEL_DEFAULT;
  universe->file_output = UNIVERSE_FILE_OUTPUT_DEFAULT;
  universe->file_substrate = UNIVERSE_FILE_SUBSTRATE_DEFAULT;
  universe->file_solvent = UNIVERSE_FILE_SOLVENT_DEFAULT;
  universe->meta_model_name = UNIVERSE_META_MODEL_NAME_DEFAULT;
  universe->meta_model_author = UNIVERSE_META_MODEL_AUTHOR_DEFAULT;
  universe->meta_model_comment = UNIVERSE_META_MODEL_COMMENT_DEFAULT;
  universe->meta_substrate_name = UNIVERSE_META_SUBSTRATE_NAME_DEFAULT;
  universe->meta_substrate_author = UNIVERSE_META_SUBSTRATE_AUTHOR_DEFAULT;
  universe->meta_substrate_comment = UNIVERSE_META_SUBSTRATE_COMMENT_DEFAULT;
  universe->meta_solvent_name = UNIVERSE_META_SOLVENT_NAME_DEFAULT;
  universe->meta_solvent_author = UNIVERSE_META_SOLVENT_AUTHOR_DEFAULT;
  universe->meta_solvent_comment = UNIVERSE_META_SOLVENT_COMMENT_DEFAULT;
  universe->substrate_atom_nb = UNIVERSE_SUBSTRATE_ATOM_NB_DEFAULT;
  universe->substrate_bond_nb = UNIVERSE_SUBSTRATE_BOND_NB_DEFAULT;
  universe->substrate_atom = UNIVERSE_SUBSTRATE_ATOM_DEFAULT;
  universe->solvent_atom_nb = UNIVERSE_SOLVENT_ATOM_NB_DEFAULT;
  universe->solvent_bond_nb = UNIVERSE_SOLVENT_BOND_NB_DEFAULT;
  universe->solvent_atom = UNIVERSE_SOLVENT_ATOM_DEFAULT;
  universe->atom = UNIVERSE_ATOM_DEFAULT;
  universe->copy_nb = UNIVERSE_COPY_NB_DEFAULT;
  universe->atom_nb = UNIVERSE_ATOM_NB_DEFAULT;
  universe->iterations = UNIVERSE_ITERATIONS_DEFAULT;
  universe->size = UNIVERSE_SIZE_DEFAULT;
  universe->time = UNIVERSE_TIME_DEFAULT;
  universe->temperature = UNIVERSE_TEMPERATURE_DEFAULT;
  universe->pressure = UNIVERSE_PRESSURE_DEFAULT;

  universe->copy_nb = args->copies;
  universe->temperature = args->temperature;
  universe->pressure = args->pressure;

  /* Open the output file */
  if ((universe->file_output = fopen(args->path_out, "w")) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  /* Open the model file */
  if ((universe->file_model = fopen(args->path_model, "r")) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  /* Get the model file's size */
  fseek(universe->file_model, 0, SEEK_END);
  file_len_model = ftell(universe->file_model);
  rewind(universe->file_model);

  /* Initialize the memory buffer for the model file */
  if ((file_buffer_model = (char*)malloc(file_len_model+1)) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  /* Load it in the model buffer, terminate the string */
  if (fread(file_buffer_model, sizeof(char), file_len_model, universe->file_model) != file_len_model)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }
  file_buffer_model[file_len_model] = '\0';

  /* Load the model from the file */
  if (universe_load_model(universe, file_buffer_model) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  /* Free the model file buffer, we're done */
  free(file_buffer_model);

  /* Open the substrate file */
  if ((universe->file_substrate = fopen(args->path_substrate, "r")) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  /* Get the substrate file's size */
  fseek(universe->file_substrate, 0, SEEK_END);
  file_len_substrate = ftell(universe->file_substrate);
  rewind(universe->file_substrate);

  /* Initialize the memory buffer for the substrate file */
  if ((file_buffer_substrate = (char*)malloc(file_len_substrate+1)) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  /* Load it in the substrate buffer, terminate the string */
  if (fread(file_buffer_substrate, sizeof(char), file_len_substrate, universe->file_substrate) != file_len_substrate)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }
  file_buffer_substrate[file_len_substrate] = '\0';

  /* Load the initial state from the substrate file */
  if (universe_load_substrate(universe, file_buffer_substrate) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  /* Free the substrate file buffer, we're done */
  free(file_buffer_substrate);
  
  /* Open the solvent file */
  if ((universe->file_solvent  = fopen(args->path_solvent, "r")) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  /* Get the solvent file's size */
  fseek(universe->file_solvent, 0, SEEK_END);
  file_len_solvent = ftell(universe->file_solvent);
  rewind(universe->file_solvent);

  /* Initialize the memory buffer for the solvent file */
  if ((file_buffer_solvent = (char*)malloc(file_len_substrate+1)) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  /* Load it in the solvent buffer, terminate the string */
  if (fread(file_buffer_solvent, sizeof(char), file_len_solvent, universe->file_solvent) != file_len_solvent)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }
  file_buffer_solvent[file_len_solvent] = '\0';

  /* Load the initial state from the solvent file */
  if (universe_load_solvent(universe, file_buffer_solvent) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  /* Free the solvent file buffer, we're done */
  free(file_buffer_solvent);

  /* Initialize the atom number */
  universe->atom_nb = (universe->substrate_atom_nb) * (universe->copy_nb);

  /* Allocate memory for the atoms */
  if ((universe->atom = malloc (sizeof(atom_t)*(universe->atom_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  /* Initialize the atom memory */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    atom_init(&(universe->atom[i]));
  }

  /* Compute the universe's size from the system density */
  /* size = cbrt(universe_mass / system_density) */
  universe_mass = 0.0;
  for (i=0; i<(universe->substrate_atom_nb); ++i)
  {
    universe_mass += (args->copies)*model_mass(universe->substrate_atom[i].element);
  }
  universe->size = cbrt((universe_mass) / (args->density));

  /* Populate the universe with extra molecules */
  if (universe_populate(universe) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  /* Enforce the PBC */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    if (atom_enforce_pbc(universe, i) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
    }
  }

  /* Apply initial velocities */
  if (universe_setvelocity(universe) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_INIT_FAILURE, __FILE__, __LINE__));
  }

  return (universe);
}

universe_t *universe_load_substrate(universe_t *universe, char *file_buffer_substrate)
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
  tok = strtok(file_buffer_substrate, "\n");
  if ((universe->meta_substrate_name = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_substrate_name, tok);

  /* Get the author line and load the author's name from it */
  tok = strtok(NULL, "\n");
  if ((universe->meta_substrate_author = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_substrate_author, tok);

  /* Get the comment line and load the author's comment from it */
  tok = strtok(NULL, "\n");
  if ((universe->meta_substrate_comment = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_substrate_comment, tok);

  /* Get the count line, and the atom/bond nb from it  */
  tok = strtok(NULL, "\n");
  sscanf(tok, "%ld %ld", &(universe->substrate_atom_nb), &(universe->substrate_bond_nb));

  /* Allocate memory for the atoms */
  if ((universe->substrate_atom = malloc (sizeof(atom_t)*(universe->substrate_atom_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }

  /* Initialize the atom memory */
  for (i=0; i<(universe->substrate_atom_nb); ++i)
  {
    atom_init(&(universe->substrate_atom[i]));
  }

  /* Read the atom block */
  for (i=0; i<(universe->substrate_atom_nb); ++i)
  {
    tok = strtok(NULL, "\n");
    sscanf(tok, "%lf %lf %lf %ld %lf %lf %lf",
           &(universe->substrate_atom[i].pos.x),
           &(universe->substrate_atom[i].pos.y),
           &(universe->substrate_atom[i].pos.z),
           &(universe->substrate_atom[i].element),
           &(universe->substrate_atom[i].charge),
           &(universe->substrate_atom[i].epsilon),
           &(universe->substrate_atom[i].sigma));

    /* Scale the atom's position vector from Å to m */
    vec3_mul(&(universe->substrate_atom[i].pos), &(universe->substrate_atom[i].pos), 1E-10);

    /* Scale the atom's charge from C.e-1 to C */
    universe->substrate_atom[i].charge *= C_ELEMCHARGE;
  }

  /* Allocate memory for the temporary bond information storage */
  if ((a1 = malloc(sizeof(uint64_t) * (universe->substrate_bond_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }
  if ((a2 = malloc(sizeof(uint64_t) * (universe->substrate_bond_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }
  if ((bond_strength = malloc(sizeof(double) * (universe->substrate_bond_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }
  /* Zero the allocated memory */
  if ((bond_index = calloc(universe->substrate_atom_nb, sizeof(uint8_t))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }

  /* Read the bond block and get the number of bonds for each atom */
  for (i=0; i<(universe->substrate_bond_nb); ++i)
  {
    tok = strtok(NULL, "\n");
    sscanf(tok, "%ld %ld %lf", &(a1[i]), &(a2[i]), &(bond_strength[i]));
    ++(universe->substrate_atom[a1[i] - 1].bond_nb);
    ++(universe->substrate_atom[a2[i] - 1].bond_nb);
  }

  /* Allocate memory for the bond information */
  for (i=0; i<(universe->substrate_atom_nb); ++i)
  {
    /* For the bond nodes */
    if ((universe->substrate_atom[i].bond = malloc(sizeof(uint64_t) * universe->substrate_atom[i].bond_nb)) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
    }

    /* For the bond strengths */
    if ((universe->substrate_atom[i].bond_strength = malloc(sizeof(double) * universe->substrate_atom[i].bond_nb)) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
    }
  }

  /* Load the bond information */
  for (i=0; i<(universe->substrate_bond_nb); ++i)
  {
    first = a1[i] - 1;
    second = a2[i] - 1;

    universe->substrate_atom[first].bond[bond_index[first]] = second;
    universe->substrate_atom[first].bond_strength[bond_index[first]] = bond_strength[i];

    universe->substrate_atom[second].bond[bond_index[second]] = first;
    universe->substrate_atom[second].bond_strength[bond_index[second]] = bond_strength[i];

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

universe_t *universe_load_solvent(universe_t *universe, char *file_buffer_solvent)
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
  tok = strtok(file_buffer_solvent, "\n");
  if ((universe->meta_solvent_name = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_solvent_name, tok);

  /* Get the author line and load the author's name from it */
  tok = strtok(NULL, "\n");
  if ((universe->meta_solvent_author = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_solvent_author, tok);

  /* Get the comment line and load the author's comment from it */
  tok = strtok(NULL, "\n");
  if ((universe->meta_solvent_comment = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_solvent_comment, tok);

  /* Get the count line, and the atom/bond nb from it  */
  tok = strtok(NULL, "\n");
  sscanf(tok, "%ld %ld", &(universe->solvent_atom_nb), &(universe->solvent_bond_nb));

  /* Allocate memory for the atoms */
  if ((universe->solvent_atom = malloc (sizeof(atom_t)*(universe->solvent_atom_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }

  /* Initialize the atom memory */
  for (i=0; i<(universe->solvent_atom_nb); ++i)
  {
    atom_init(&(universe->solvent_atom[i]));
  }

  /* Read the atom block */
  for (i=0; i<(universe->solvent_atom_nb); ++i)
  {
    tok = strtok(NULL, "\n");
    sscanf(tok, "%lf %lf %lf %ld %lf %lf %lf",
           &(universe->solvent_atom[i].pos.x),
           &(universe->solvent_atom[i].pos.y),
           &(universe->solvent_atom[i].pos.z),
           &(universe->solvent_atom[i].element),
           &(universe->solvent_atom[i].charge),
           &(universe->solvent_atom[i].epsilon),
           &(universe->solvent_atom[i].sigma));

    /* Scale the atom's position vector from Å to m */
    vec3_mul(&(universe->solvent_atom[i].pos), &(universe->solvent_atom[i].pos), 1E-10);

    /* Scale the atom's charge from C.e-1 to C */
    universe->solvent_atom[i].charge *= C_ELEMCHARGE;
  }

  /* Allocate memory for the temporary bond information storage */
  if ((a1 = malloc(sizeof(uint64_t) * (universe->solvent_bond_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }
  if ((a2 = malloc(sizeof(uint64_t) * (universe->solvent_bond_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }
  if ((bond_strength = malloc(sizeof(double) * (universe->solvent_bond_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }
  /* Zero the allocated memory */
  if ((bond_index = calloc(universe->solvent_atom_nb, sizeof(uint8_t))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
  }

  /* Read the bond block and get the number of bonds for each atom */
  for (i=0; i<(universe->solvent_bond_nb); ++i)
  {
    tok = strtok(NULL, "\n");
    sscanf(tok, "%ld %ld %lf", &(a1[i]), &(a2[i]), &(bond_strength[i]));
    ++(universe->solvent_atom[a1[i] - 1].bond_nb);
    ++(universe->solvent_atom[a2[i] - 1].bond_nb);
  }

  /* Allocate memory for the bond information */
  for (i=0; i<(universe->solvent_atom_nb); ++i)
  {
    /* For the bond nodes */
    if ((universe->solvent_atom[i].bond = malloc(sizeof(uint64_t) * universe->solvent_atom[i].bond_nb)) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
    }

    /* For the bond strengths */
    if ((universe->solvent_atom[i].bond_strength = malloc(sizeof(double) * universe->solvent_atom[i].bond_nb)) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_FAILURE, __FILE__, __LINE__));
    }
  }

  /* Load the bond information */
  for (i=0; i<(universe->solvent_bond_nb); ++i)
  {
    first = a1[i] - 1;
    second = a2[i] - 1;

    universe->solvent_atom[first].bond[bond_index[first]] = second;
    universe->solvent_atom[first].bond_strength[bond_index[first]] = bond_strength[i];

    universe->solvent_atom[second].bond[bond_index[second]] = first;
    universe->solvent_atom[second].bond_strength[bond_index[second]] = bond_strength[i];

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
  vec3_t pos_offset;
  atom_t *reference;
  atom_t *duplicate;

  for (i=0; i<(universe->copy_nb); ++i)
  {
    /* Generate a random position vector to load the system at */
    vec3_marsaglia(&pos_offset);
    vec3_mul(&pos_offset, &pos_offset, (1-UNIVERSE_POPULATE_MIN_DIST)*(universe->size)*cos(rand()) + UNIVERSE_POPULATE_MIN_DIST*(universe->size));

    /* Load each atom from the reference system into the universe */
    for (ii=0; ii<(universe->substrate_atom_nb); ++ii)
    {
      /* Just shortcuts, they make the code cleaner */
      reference = &(universe->substrate_atom[ii]);
      duplicate = &(universe->atom[(i*(universe->substrate_atom_nb)) + ii]);

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
      vec3_add(&(duplicate->pos), &(reference->pos), &pos_offset);

      /* Allocate memory for the bond information */
      if ((duplicate->bond = malloc(sizeof(uint64_t)*(reference->bond_nb))) == NULL)
      {
        return (retstr(NULL, TEXT_UNIVERSE_POPULATE_FAILURE, __FILE__, __LINE__));
      }
      if ((duplicate->bond_strength = malloc(sizeof(double)*(reference->bond_nb))) == NULL)
      {
        return (retstr(NULL, TEXT_UNIVERSE_POPULATE_FAILURE, __FILE__, __LINE__));
      }

      /* Load the bond information */
      for (iii=0; iii<(duplicate->bond_nb); ++iii)
      {
        duplicate->bond[iii] = reference->bond[iii] + (i*(universe->substrate_atom_nb));
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
  vec3_t vec;     /* Random vector */

  /* Get the molecular mass */
  mass_mol = 0;
  for (i=0; i<(universe->substrate_atom_nb); ++i)
  {
    mass_mol += model_mass(universe->substrate_atom[i].element);
  }

  /* Get the average velocity */
  velocity = sqrt(3*C_BOLTZMANN*(universe->temperature)/mass_mol);

  /* For every atom in the universe */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    /* Apply the velocity in a random direction */
    vec3_marsaglia(&vec);
    vec3_mul(&(universe->atom[i].vel), &vec, velocity);
  }

  return (universe);
}

void universe_clean(universe_t *universe)
{
  size_t i;

  /* Clean each atom in the reference system */
  for (i=0; i<(universe->substrate_atom_nb); ++i)
  {
    atom_clean(&(universe->substrate_atom[i]));
  }

  /* Clean each atom in the universe */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    atom_clean(&(universe->atom[i]));
  }

  /* Close the file pointers */
  fclose(universe->file_substrate);
  fclose(universe->file_solvent);
  fclose(universe->file_output);

  /* Free allocated memory */
  free(universe->meta_substrate_name);
  free(universe->meta_substrate_author);
  free(universe->meta_substrate_comment);
  free(universe->substrate_atom);
  free(universe->meta_solvent_name);
  free(universe->meta_solvent_author);
  free(universe->meta_solvent_comment);
  free(universe->solvent_atom);
  free(universe->atom);
}

/* Main loop of the simulator. Iterates until the target time is reached */
int universe_simulate(universe_t *universe, const args_t *args)
{
  uint64_t frame_nb; /* Used for frameskipping */

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
      {
        return (retstri(EXIT_FAILURE, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));
      }
      frame_nb = (args->frameskip);
    }
    else
      --frame_nb;

    /* Iterate */
    if (universe_iterate(universe, args) == NULL)
    {
      return (retstri(EXIT_FAILURE, TEXT_UNIVERSE_SIMULATE_FAILURE, __FILE__, __LINE__));
    }

    universe->time += args->timestep;
    ++(universe->iterations);
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
  {
    if (atom_update_pos(universe, args, i) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
    }
  }

  /* We enforce the periodic boundary conditions */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    if (atom_enforce_pbc(universe, i) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
    }
  }

  /* Update the force vectors */
  /* By numerically differentiating the potential energy... 'toute facon, entre ma consommation electrique, mes achats inter*/
  if (args->numerical == MODE_NUMERICAL)
  {
    for (i=0; i<(universe->atom_nb); ++i)
    {
      if (atom_update_frc_numerical(universe, i) == NULL)
      {
        return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
      }
    }
  }

  /* By numerically differentiating the potential energy using points in a tetrahedron... */
  if (args->numerical == MODE_NUMERICAL_TETRA)
  {
    for (i=0; i<(universe->atom_nb); ++i)
      if (atom_update_frc_numerical_tetrahedron(universe, i) == NULL)
        return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
  }

  /* Or analytically solving for force */
  else
  {
    for (i=0; i<(universe->atom_nb); ++i)
    {
      if (atom_update_frc_analytical(universe, i) == NULL)
      {
        return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
      }
    }
  }

  /* Update the acceleration vectors */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    if (atom_update_acc(universe, i) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
    }
  }

  /* Update the speed vectors */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    if (atom_update_vel(universe, args, i) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_ITERATE_FAILURE, __FILE__, __LINE__));
    }
  }

  return (universe);
}

/* Print the system's state to the .xyz file */
universe_t *universe_printstate(universe_t *universe)
{
  size_t i; /* Iterator */

  /* Print in the .xyz */
  fprintf(universe->file_output, "%ld\n%ld\n", universe->atom_nb, universe->iterations);
  for (i=0; i<(universe->atom_nb); ++i)
  {
    fprintf(universe->file_output,
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
    vel = vec3_mag(&(universe->atom[i].vel));
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
    {
      return (retstr(NULL, TEXT_UNIVERSE_ENERGY_POTENTIAL_FAILURE, __FILE__, __LINE__));
    }
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
  {
    return (retstr(NULL, TEXT_UNIVERSE_ENERGY_TOTAL_FAILURE, __FILE__, __LINE__));
  }

  /* Get the potential energy */
  if (universe_energy_potential(universe, &potential) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_ENERGY_TOTAL_FAILURE, __FILE__, __LINE__));
  }

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
  {
    return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
  }
  printf(TEXT_POTENTIAL, potential*1E12);

  /* Print the target potential */
  printf(TEXT_REDUCEPOT, args->reduce_potential*1E12);

  /* PHASE 1 - BRUTEFORCE */
  while (potential > UNIVERSE_REDUCEPOT_COARSE_THRESHOLD * args->reduce_potential)
  {
    if (universe_reducepot_coarse(universe) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
    }

    /* Update the system's potential energy */
    if (universe_energy_potential(universe, &potential) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
    }
  }

  /* PHASE 2 - GRADIENT DESCENT */
  while (potential > args->reduce_potential)
  {
    if (universe_reducepot_fine(universe) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
    }

    /* Update the system's potential energy */
    if (universe_energy_potential(universe, &potential) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FAILURE, __FILE__, __LINE__));
    }
  }

  return (universe);
}

/* Apply transformations to lower the system's potential energy (wiggling) */
universe_t *universe_reducepot_coarse(universe_t *universe)
{
  size_t i;
  size_t tries;
  double step_magnitude;
  double pot_pre;
  double pot_post;
  vec3_t step;
  vec3_t pos_pre;

  /* For each atom */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    /* Backup the coordinates */
    pos_pre.x = universe->atom[i].pos.x;
    pos_pre.y = universe->atom[i].pos.y;
    pos_pre.z = universe->atom[i].pos.z;

    /* Compute the pre-transformation potential */
    if (universe_energy_total(universe, &pot_pre) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_COARSE_FAILURE, __FILE__, __LINE__));
    }

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
      vec3_marsaglia(&step);
      vec3_mul(&step, &step, step_magnitude);

      /* Apply the displacement */
      vec3_add(&(universe->atom[i].pos), &(universe->atom[i].pos), &step);

      /* Enforce PBCs */
      if (atom_enforce_pbc(universe, i) == NULL)
      {
        return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_COARSE_FAILURE, __FILE__, __LINE__));
      }

      /* Compute the post-transformation potential */
      if (universe_energy_total(universe, &pot_post) == NULL)
      {
        return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_COARSE_FAILURE, __FILE__, __LINE__));
      }
    } while (pot_post > pot_pre);
  }

  return (universe);
}

/* Apply transformations to lower the system's potential energy (gradient descent) */
universe_t *universe_reducepot_fine(universe_t *universe)
{
  size_t i;
  double step_magnitude;
  double pot_pre;
  double pot_post;
  vec3_t step;
  vec3_t pos_pre;

  /* For each atom */
  for (i=0; i<(universe->atom_nb); ++i)
  {
    /* Backup the coordinates */
    pos_pre = universe->atom[i].pos;

    /* Compute the potential gradient with respect to the atom's coordinates (=force) */
    if (atom_update_frc_analytical(universe, i) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FINE_FAILURE, __FILE__, __LINE__));
    }

    /* Compute the potential before the transformation */
    if (universe_energy_potential(universe, &pot_pre) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FINE_FAILURE, __FILE__, __LINE__));
    }

    /* "Take a step" toward the local minima*/
    /* The magnitude is derived from the displacement resulting from a constant acceleration */
    /* displacement = acceleration * t^2 * 0.5 */
    /*              = force * t^2 / (2 * mass) */
    /* The direction in which the step is taken is derived from the force vector, since force = -nabla*potential */
    /* Motion is just fancy gradient descent that instead of bleeding potential conserves it as kinetic energy */
    /* Think of this algorithm as a simulation without motion, we're just reaching equilibrium without motion */
    step_magnitude = POW2(UNIVERSE_REDUCEPOT_FINE_TIMESTEP)/(2*model_mass(universe->atom[i].element));
    vec3_mul(&step, &(universe->atom[i].frc), step_magnitude);

    /* Limit the maximum displacement to 1 Angstrom */
    if (vec3_mag(&step) > UNIVERSE_REDUCEPOT_FINE_MAX_STEP)
    {
      vec3_mul(&step, &step, UNIVERSE_REDUCEPOT_FINE_MAX_STEP/vec3_mag(&step));
    }

    /* Apply the transformation */
    vec3_add(&(universe->atom[i].pos), &(universe->atom[i].pos), &step);

    /* Enforce PBCs */
    if (atom_enforce_pbc(universe, i) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FINE_FAILURE, __FILE__, __LINE__));
    }

    /* Compute the potential after the transformation */
    if (universe_energy_potential(universe, &pot_post) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_REDUCEPOT_FINE_FAILURE, __FILE__, __LINE__));
    }

    /* If the potential increased, discard the transformation */
    if (pot_post > pot_pre)
    {
      universe->atom[i].pos = pos_pre;
    }
  }

  return (universe);
}

universe_t *universe_parameters_print(universe_t *universe, const args_t *args)
{
  double potential;    /* The universe's potential energy */

  /* Compute the potential energy */
   if (universe_energy_potential(universe, &potential) == NULL)
   {
     return (retstr(NULL, TEXT_UNIVERSE_PARAMETERS_PRINT_FAILURE, __FILE__, __LINE__));
   }
  
  /* Print some useful information */
  puts(TEXT_INFO_MODEL);
  printf(TEXT_INFO_PATH, "todo");
  printf(TEXT_INFO_NAME, "todo");
  printf(TEXT_INFO_AUTHOR, "todo");
  printf(TEXT_INFO_COMMENT, "todo");
  puts("\n");

  puts(TEXT_INFO_SUBSTRATE);
  printf(TEXT_INFO_PATH, args->path_substrate);
  printf(TEXT_INFO_NAME, universe->meta_substrate_name);
  printf(TEXT_INFO_AUTHOR, universe->meta_substrate_author);
  printf(TEXT_INFO_COMMENT, universe->meta_substrate_comment);
  printf(TEXT_INFO_SUBSTRATE_ATOM_NB, universe->substrate_atom_nb);
  printf(TEXT_INFO_REF_BOND_NB, universe->substrate_bond_nb);
  puts("\n");

  puts(TEXT_INFO_SOLVENT);
  printf(TEXT_INFO_PATH, args->path_solvent);
  printf(TEXT_INFO_NAME, universe->meta_solvent_name);
  printf(TEXT_INFO_AUTHOR, universe->meta_solvent_author);
  printf(TEXT_INFO_COMMENT, universe->meta_solvent_comment);
  printf(TEXT_INFO_SUBSTRATE_ATOM_NB, universe->solvent_atom_nb);
  printf(TEXT_INFO_REF_BOND_NB, universe->solvent_bond_nb);
  puts("\n");

  puts(TEXT_INFO_SIMULATION);
  printf(TEXT_INFO_SUBSTRATE_COPIES, universe->copy_nb);
  printf(TEXT_INFO_ATOM_NB, universe->atom_nb);
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
