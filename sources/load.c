/*
 * load.c
 *
 * Licensed under GPLv3 license
 *
 */

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "model.h"
#include "text.h"
#include "util.h"
#include "universe.h"

universe_t *universe_load_model(universe_t *universe, char *file_buffer_model)
{
  char *tok;
  size_t i;

  /* Get the number of entries in the model */
  /* -3 is there because we're not counting the metadata lines */
  universe->model.entry_nb = line_nb(file_buffer_model) - 4;

  /* Allocate memory for the entries */
  if ((universe->model.entry = malloc(sizeof(model_entry_t) * universe->model.entry_nb)) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_MODEL_FAILURE, __FILE__, __LINE__));
  }

  /* Initialize each entry */
  for (i=0; i < universe->model.entry_nb; ++i)
  {
    if (model_entry_init(&(universe->model.entry[i])) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_MODEL_FAILURE, __FILE__, __LINE__));
    }
  }

  /* Get the name line and load the model's name from it */
  tok = strtok(file_buffer_model, "\n");
  if ((universe->meta_model_name = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_MODEL_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_model_name, tok);

  /* Get the author line and load the author's name from it */
  tok = strtok(NULL, "\n");
  if ((universe->meta_model_author = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_MODEL_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_model_author, tok);

  /* Get the comment line and load the author's comment from it */
  tok = strtok(NULL, "\n");
  if ((universe->meta_model_comment = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_MODEL_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_model_comment, tok);

  /* Load each entry */
  for (i=0; i < (universe->model.entry_nb); ++i)
  {
    /* Get the next line */
    tok = strtok(NULL, "\n");

    /* Initialize memory for each entry name and symbol */
    if ((universe->model.entry[i].name = malloc(sizeof(char) * strlen(tok))) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_MODEL_FAILURE, __FILE__, __LINE__));
    }

    if ((universe->model.entry[i].symbol = malloc(sizeof(char) * strlen(tok))) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_MODEL_FAILURE, __FILE__, __LINE__));
    }

    /* Get the next entry as a line */
    if (sscanf(tok, "%s %s %lf %lf %lf %lf %lf %lf",
           universe->model.entry[i].name,
           universe->model.entry[i].symbol,
           &(universe->model.entry[i].mass),
           &(universe->model.entry[i].radius_covalent),
           &(universe->model.entry[i].radius_vdw),
           &(universe->model.entry[i].bond_angle),
           &(universe->model.entry[i].lj_epsilon),
           &(universe->model.entry[i].lj_sigma)) != 8)
    {
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_MODEL_FAILURE, __FILE__, __LINE__));
    }
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
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SUBSTRATE_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_substrate_name, tok);

  /* Get the author line and load the author's name from it */
  tok = strtok(NULL, "\n");
  if ((universe->meta_substrate_author = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SUBSTRATE_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_substrate_author, tok);

  /* Get the comment line and load the author's comment from it */
  tok = strtok(NULL, "\n");
  if ((universe->meta_substrate_comment = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SUBSTRATE_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_substrate_comment, tok);

  /* Get the count line, and the atom/bond nb from it  */
  tok = strtok(NULL, "\n");
  sscanf(tok, "%ld %ld", &(universe->substrate_atom_nb), &(universe->substrate_bond_nb));

  /* Allocate memory for the atoms */
  if ((universe->substrate_atom = malloc (sizeof(atom_t)*(universe->substrate_atom_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SUBSTRATE_FAILURE, __FILE__, __LINE__));
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
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SUBSTRATE_FAILURE, __FILE__, __LINE__));
  }
  if ((a2 = malloc(sizeof(uint64_t) * (universe->substrate_bond_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SUBSTRATE_FAILURE, __FILE__, __LINE__));
  }
  if ((bond_strength = malloc(sizeof(double) * (universe->substrate_bond_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SUBSTRATE_FAILURE, __FILE__, __LINE__));
  }
  /* Zero the allocated memory */
  if ((bond_index = calloc(universe->substrate_atom_nb, sizeof(uint8_t))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SUBSTRATE_FAILURE, __FILE__, __LINE__));
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
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_SUBSTRATE_FAILURE, __FILE__, __LINE__));
    }

    /* For the bond strengths */
    if ((universe->substrate_atom[i].bond_strength = malloc(sizeof(double) * universe->substrate_atom[i].bond_nb)) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_SUBSTRATE_FAILURE, __FILE__, __LINE__));
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
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SOLVENT_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_solvent_name, tok);

  /* Get the author line and load the author's name from it */
  tok = strtok(NULL, "\n");
  if ((universe->meta_solvent_author = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SOLVENT_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_solvent_author, tok);

  /* Get the comment line and load the author's comment from it */
  tok = strtok(NULL, "\n");
  if ((universe->meta_solvent_comment = malloc(sizeof(char)*(strlen(tok)+1))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SOLVENT_FAILURE, __FILE__, __LINE__));
  }
  strcpy(universe->meta_solvent_comment, tok);

  /* Get the count line, and the atom/bond nb from it  */
  tok = strtok(NULL, "\n");
  sscanf(tok, "%ld %ld", &(universe->solvent_atom_nb), &(universe->solvent_bond_nb));

  /* Allocate memory for the atoms */
  if ((universe->solvent_atom = malloc (sizeof(atom_t)*(universe->solvent_atom_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SOLVENT_FAILURE, __FILE__, __LINE__));
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
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SOLVENT_FAILURE, __FILE__, __LINE__));
  }
  if ((a2 = malloc(sizeof(uint64_t) * (universe->solvent_bond_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SOLVENT_FAILURE, __FILE__, __LINE__));
  }
  if ((bond_strength = malloc(sizeof(double) * (universe->solvent_bond_nb))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SOLVENT_FAILURE, __FILE__, __LINE__));
  }
  /* Zero the allocated memory */
  if ((bond_index = calloc(universe->solvent_atom_nb, sizeof(uint8_t))) == NULL)
  {
    return (retstr(NULL, TEXT_UNIVERSE_LOAD_SOLVENT_FAILURE, __FILE__, __LINE__));
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
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_SOLVENT_FAILURE, __FILE__, __LINE__));
    }

    /* For the bond strengths */
    if ((universe->solvent_atom[i].bond_strength = malloc(sizeof(double) * universe->solvent_atom[i].bond_nb)) == NULL)
    {
      return (retstr(NULL, TEXT_UNIVERSE_LOAD_SOLVENT_FAILURE, __FILE__, __LINE__));
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
